#include <ctype.h>
#include <driver/uart.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <vy.h>

static size_t callback_placeholder(void *ctx, const char *string, size_t length) {
	(void) ctx;
	(void) string;

	return length;
}

int vcbprintf(void *ctx, size_t (*callback)(void *ctx, const char *string, size_t length), const char *format, va_list args) {
	if(!callback) {
		callback = callback_placeholder;
	}

	if(!format || (uintptr_t) format == 0xffffffffffffffff) {
		return 0;
	}

	size_t written = 0;
	bool bad_conversion = false;

	while(*format) {
		if(*format != '%') {
			size_t block;
		normal_print:
			block = 1;

			while(format[block] && format[block] != '%') {
				block++;
			}

			if(callback(ctx, format, block) != block) {
				return -1;
			}

			format += block;
			written += block;
			continue;
		}

		const char *format_start = format;

		if(*++format == '%') {
			goto normal_print;
		}

		if(bad_conversion) {
		unknown_conversion:
			bad_conversion = true;
			format = format_start;
			goto normal_print;
		}

		bool flag_hash = false;
		bool flag_minus = false;
		bool flag_plus = false;
		bool flag_space = false;
		bool flag_zero = false;

		while(*format) {
			switch(*format++) {
				case '#': flag_hash = true; continue;
				case '-': flag_minus = true; continue;
				case '+': flag_plus = true; continue;
				case ' ': flag_space = true; continue;
				case '0': flag_zero = true; continue;
				default: format--; break;
			}

			break;
		}

		char pad_char = (flag_zero) ? '0' : ' ';
		size_t width = 0;

	width_read:
		if(*format >= '0' && *format <= '9') {
			width *= 10;
			width += (size_t) (*format - '0');
			format++;
			goto width_read;
		} else if(*format == '*') {
			width = (size_t) va_arg(args, int);
			format++;
		}

		size_t precision = (size_t) -1;

		if(*format == '.') {
			format++;
			precision = 0;

			while(*format >= '0' && *format <= '9') {
				precision *= 10;
				precision += (size_t) (*format - '0');
				format++;
			}
		}

		enum length {
			LENGTH_SHORT_SHORT,
			LENGTH_SHORT,
			LENGTH_DEFAULT,
			LENGTH_LONG,
			LENGTH_LONG_LONG,
			LENGTH_LONG_DOUBLE,
			LENGTH_INTMAX_T,
			LENGTH_SIZE_T,
			LENGTH_PTRDIFF_T,
		};

		struct length_modifer {
			const char* name;
			enum length length;
			size_t name_length;
		};

		struct length_modifer length_modifiers[] = {
			{ "hh", LENGTH_SHORT_SHORT, 2 },
			{ "h", LENGTH_SHORT, 1 },
			{ "", LENGTH_DEFAULT, 0 },
			{ "l", LENGTH_LONG, 1 },
			{ "ll", LENGTH_LONG_LONG, 2 },
			{ "L", LENGTH_LONG_DOUBLE, 1 },
			{ "j", LENGTH_INTMAX_T, 1 },
			{ "z", LENGTH_SIZE_T, 1 },
			{ "t", LENGTH_PTRDIFF_T, 1 },
		};

		enum length length = LENGTH_DEFAULT;
		size_t length_length = 0;

		for(size_t i = 0; i < sizeof(length_modifiers) / sizeof(length_modifiers[0]); i++) {
			if(length_modifiers[i].name_length < length_length) {
				continue;
			}

			if(strncmp(format, length_modifiers[i].name, length_modifiers[i].name_length) != 0) {
				continue;
			}

			length = length_modifiers[i].length;
			length_length = length_modifiers[i].name_length;
		}

		format += length_length;

		switch(*format) {
			case '%': {
				if(callback(ctx, format, 1) != 1) {
					return -1;
				}

				format++;

				break;
			}
			case 's': {
				const char *string = va_arg(args, const char *);
				size_t len = strlen(string);
				len = (precision < len) ? precision : len;

				if(callback(ctx, string, len) != len) {
					return -1;
				}

				written += len;
				format++;

				break;
			}
			case 'c': {
				char c = (char) va_arg(args, int);

				if(callback(ctx, &c, 1) != 1) {
					return -1;
				}

				format++;
				written++;

				break;
			}
			case 'd':
			case 'i':
			case 'o':
			case 'p':
			case 'u':
			case 'x':
			case 'X': {
				char specifier = *format++;

				uintmax_t value;
				bool negative = false;

				if(specifier == 'p') {
					value = (uintptr_t) va_arg(args, void *);
					specifier = 'x';
				} else if(specifier == 'i' || specifier == 'd') {
					intmax_t signed_value;

					if(length == LENGTH_SHORT_SHORT) {
						signed_value = va_arg(args, int);
					} else if(length == LENGTH_SHORT) {
						signed_value = va_arg(args, int);
					} else if(length == LENGTH_DEFAULT) {
						signed_value = va_arg(args, int);
					} else if(length == LENGTH_LONG) {
						signed_value = va_arg(args, long);
					} else if(length == LENGTH_LONG_LONG) {
						signed_value = va_arg(args, long long);
					} else if(length == LENGTH_INTMAX_T) {
						signed_value = va_arg(args, intmax_t);
					} else if(length == LENGTH_SIZE_T) {
						signed_value = va_arg(args, ssize_t);
					} else if(length == LENGTH_PTRDIFF_T) {
						signed_value = va_arg(args, ptrdiff_t);
					} else {
						goto unknown_conversion;
					}

					value = (negative = (signed_value < 0)) ? (- (uintmax_t) signed_value) : (uintmax_t) signed_value;
				} else {
					if(length == LENGTH_SHORT_SHORT) {
						value = va_arg(args, unsigned int);
					} else if(length == LENGTH_SHORT) {
						value = va_arg(args, unsigned int);
					} else if(length == LENGTH_DEFAULT) {
						value = va_arg(args, unsigned int);
					} else if(length == LENGTH_LONG) {
						value = va_arg(args, unsigned long);
					} else if(length == LENGTH_LONG_LONG) {
						value = va_arg(args, unsigned long long);
					} else if(length == LENGTH_INTMAX_T) {
						value = va_arg(args, uintmax_t);
					} else if(length == LENGTH_SIZE_T) {
						value = va_arg(args, size_t);
					} else if(length == LENGTH_PTRDIFF_T) {
						value = (uintmax_t) va_arg(args, ptrdiff_t);
					} else {
						goto unknown_conversion;
					}
				}

				/* handle the prefix */
				char prefix[3];
				size_t prefix_len = 0;

				if(negative) {
					prefix[prefix_len++] = '-';
				} else if(flag_plus) {
					/* flag_plus overrides flag_space */
					prefix[prefix_len++] = '+';
				} else if(flag_space) {
					prefix[prefix_len++] = ' ';
				}

				if(flag_hash && (specifier == 'x' || specifier == 'X')) {
					prefix[prefix_len++] = '0';
					prefix[prefix_len++] = specifier;
				} else if(flag_hash && (specifier == 'o')) {
					prefix[prefix_len++] = '0';
				}

				prefix[prefix_len] = 0;

				/* handle integer to string conversion */
				char buf[21];
				const char *conv = (specifier == 'X') ? "0123456789ABCDEF" : "0123456789abcdef";
				uintmax_t base = (specifier == 'X' || specifier == 'x') ? 16 : (specifier == 'o' ? 8 : 10);

				size_t buf_length = 1;
				uintmax_t copy = value;

				while(base <= copy) {
					copy /= base;
					buf_length++;
				}

				size_t printed_width = (precision != (size_t) -1 && precision > buf_length) ? precision : buf_length;
				size_t pad = ((printed_width + prefix_len) > width) ? 0 : width - (printed_width + prefix_len);

				for(size_t i = buf_length; i > 0; i--) {
					buf[i-1] = conv[value % base];
					value /= base;
				}

				buf[buf_length] = 0;

				if(pad_char == ' ') {
					const char *pad_string = " ";
					for(size_t i = 0; i < pad; i++) {
						if(callback(ctx, pad_string, 1) != 1) {
							return -1;
						}
					}
				}

				/* print the prefix (if any) */
				if(callback(ctx, prefix, prefix_len) != prefix_len) {
					return -1;
				}

				if(pad_char == '0' && !flag_minus) {
					const char *pad_string = "0";
					for(size_t i = 0; i < pad; i++) {
						if(callback(ctx, pad_string, 1) != 1) {
							return -1;
						}
					}
				}

				if(printed_width > buf_length) {
					for(size_t i = 0; i < (printed_width - buf_length); i++) {
						if(callback(ctx, "0", 1) != 1) {
							return -1;
						}
					}
				}

				/* print the integer string */
				if(callback(ctx, buf, buf_length) != buf_length) {
					return -1;
				}

				if(flag_minus) {
					const char *pad_string = " ";
					for(size_t i = 0; i < pad; i++) {
						if(callback(ctx, pad_string, 1) != 1) {
							return -1;
						}
					}
				}

				written += buf_length;

				break;
			}
			default: {
				goto unknown_conversion;
			}
		}
	}

	if(written > INT_MAX) {
		return -1;
	}

	return (int) written;
}
