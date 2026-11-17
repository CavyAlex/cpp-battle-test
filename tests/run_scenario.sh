#!/usr/bin/env bash
# Прогон одного сценария. Ожидания записаны в самом файле:
#   // EXPECT: <строка лога>        — stdout должен совпасть построчно (пробелы в конце строк не учитываются)
#   // EXPECT-ERROR: <фрагмент>     — программа должна завершиться с ошибкой, а stderr — содержать фрагмент
#   // SEED: <число>                — seed генератора случайных чисел (по умолчанию 1)
#   // INPUT: <путь>                — команды взять из другого файла (путь относительно сценария)
#   // EXPECT-REPRODUCIBLE          — два прогона с одним seed должны дать одинаковый непустой лог
set -u
binary=$1
scenario=$2
input=$(sed -n 's|^// INPUT: ||p' "$scenario")
input=${input:+$(dirname "$scenario")/$input}
input=${input:-$scenario}
seed=$(sed -n 's|^// SEED: ||p' "$scenario")
seed=${seed:-1}

if grep -q '^// EXPECT-REPRODUCIBLE' "$scenario"; then
	first=$("$binary" "$input" --seed "$seed") || exit 1
	second=$("$binary" "$input" --seed "$seed") || exit 1
	if [[ -z "$first" || "$first" != "$second" ]]; then
		echo "Runs with seed $seed differ or are empty"
		diff <(echo "$first") <(echo "$second")
		exit 1
	fi
	exit 0
fi

expectedError=$(sed -n 's|^// EXPECT-ERROR: ||p' "$scenario")
if [[ -n "$expectedError" ]]; then
	actualError=$("$binary" "$input" --seed "$seed" 2>&1 >/dev/null)
	status=$?
	if [[ $status -eq 0 ]]; then
		echo "Expected failure with: $expectedError"
		exit 1
	fi
	if [[ "$actualError" != *"$expectedError"* ]]; then
		echo "Expected error: $expectedError"
		echo "Actual error:   $actualError"
		exit 1
	fi
	exit 0
fi

actual=$("$binary" "$input" --seed "$seed")
status=$?
if [[ $status -ne 0 ]]; then
	echo "Unexpected exit code $status"
	exit 1
fi
expected=$(sed -n 's|^// EXPECT: ||p' "$scenario" | sed 's/[[:space:]]*$//')
actual=$(echo "$actual" | sed 's/[[:space:]]*$//')
if [[ "$actual" != "$expected" ]]; then
	diff <(echo "$expected") <(echo "$actual")
	exit 1
fi
