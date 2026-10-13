#!/usr/bin/env bash
# List unresolved non-Python/libc/GL symbols of built extension modules.
cd "$(dirname "$0")/.."
if [ $# -gt 0 ]; then files=("$@"); else mapfile -t files < <(find src -name '*.so'); fi
for s in "${files[@]}"; do
  u=$(nm -u "$s" | awk '{print $2}' | grep -vE '^(Py|_Py|gl|glu|_ITM|__gmon|_Jv|__cxa|_Unwind|_ZN|_ZS|_ZT|__gxx|__stack_chk|__assert|__errno|__ctype|__isoc|__printf|__sprintf|__snprintf|__fprintf|__vfprintf|__memcpy|__memset|__strcpy|__strcat|__fread|__fdelt|__longjmp)' | grep -v '@' )
  out=""
  for sym in $u; do
    case $sym in
      malloc|free|calloc|realloc|memcpy|memset|memmove|strcmp|strcpy|strlen|strncpy|strcat|strncmp|strchr|strrchr|strstr|strtok|strdup|sprintf|printf|fprintf|snprintf|puts|putchar|fopen|fclose|fread|fwrite|fgets|fputs|fflush|fseek|ftell|rewind|feof|sscanf|fscanf|exit|abort|qsort|atoi|atof|atol|strtol|strtod|sqrt|sqrtf|pow|powf|sin|cos|tan|atan|atan2|acos|asin|exp|log|log10|fabs|fabsf|floor|ceil|fmod|rand|srand|random|srandom|drand48|time|clock|getenv|memcmp|toupper|tolower|isspace|isdigit|isalpha|stdout|stderr|stdin|vfprintf|vsprintf|vsnprintf|perror|remove|rename|tmpfile|getc|ungetc|fputc|fgetc|xdr_*|gettimeofday|getrusage|times|sinf|cosf|tanf|atanf|acosf|expf|logf|floorf|ceilf|roundf|round|rint|lrint|ldexp|frexp|hypot|cbrt|erf|isnan|isinf|strerror|setvbuf|fileno|fdopen|access|unlink|stat|fstat|open|close|read|write|lseek|signal|usleep|sleep|__isnan*|__isinf*|__finite*|*@*) ;;
      *) out="$out $sym";;
    esac
  done
  [ -n "$out" ] && echo "$s:$out"
done
true
