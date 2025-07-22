#!/usr/bin/env bash

set -e

export PATH="$PATH:$(dirname $(realpath ${BASH_SOURCE[0]}))/build"

fatal() {
	>&2 echo "Error: $*"
	exit 1
}

[ -z "$1" ] && fatal "Missing filename(s)"

declare -A fds

while [ -n "$1" ]; do
	file="$1"

	if [ "$file" = "stdin" ]; then
		fds["0"]="stdin"
		shift
		continue
	fi

	[ -p "$file" ] && rm "$file"
	[ -e "$file" ] && fatal "$file exists and is not a named pipe"

	mkfifo "$file"

	exec {fd}<>"$file"
	fds["$fd"]="$file"

	shift
done

while true; do
	IFS='\n' read -ra ready_fds < <(poll "${!fds[@]}")

	[[ "${#ready_fds[@]}" == 0 ]] && exit

	for fd in "${ready_fds[@]}"; do
		read -ru "$fd" line
		echo "${fds["$fd"]}: $line"
	done
done
