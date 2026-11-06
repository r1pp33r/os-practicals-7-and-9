#!/usr/bin/env bash
set -o pipefail
fail() { printf 'Error: %s\n' "$*" >&2; return 1; }
help() {
  printf '%s\n' 'Usage: sysinfo.sh [OPTIONS]' \
    '-u, --users       Users and home directories, alphabetical order' \
    '-p, --processes   Running processes, ordered by PID' \
    '-h, --help        Print help and stop' \
    '-l, --log PATH    Write normal output to PATH' \
    '-e, --errors PATH Write errors to PATH'
}
users() { getent passwd | cut -d: -f1,6 | LC_ALL=C sort; }
processes() { ps -eo pid=,comm= --sort=pid; }
redirect() {
  local path=$1 stream=$2 parent
  [[ -n $path && ! -d $path ]] || { fail "invalid output path: $path"; return 1; }
  parent=$(dirname -- "$path")
  [[ -d $parent && -x $parent && -w $parent ]] || { fail "directory not writable: $parent"; return 1; }
  [[ ! -e $path || -w $path ]] || { fail "file not writable: $path"; return 1; }
  # Suppress shell diagnostics only while opening; report errors consistently.
  local fd
  if ! { exec {fd}>"$path"; } 2>/dev/null; then fail "cannot open: $path"; return 1; fi
  if [[ $stream == 1 ]]; then exec 1>&$fd; else exec 2>&$fd; fi
  exec {fd}>&-
}
pending=("$@")
for ((i=0;i<${#pending[@]};i++)); do
  [[ ${pending[i]} != -- ]] || break
  if [[ ${pending[i]} == -e || ${pending[i]} == --errors ]]; then
    ((i+1<${#pending[@]})) || { fail 'missing errors path'; exit 2; }
    redirect "${pending[i+1]}" 2 || exit 1
    ((i++))
  fi
done
parsed=$(getopt -o upl:e:h -l users,processes,log:,errors:,help -- "$@" 2>/dev/null) || { fail 'unknown option or missing argument; use --help'; exit 2; }
eval set -- "$parsed"
actions=(); log=''; errors=''; show_help=0
while true; do
  case $1 in
    -u|--users) actions+=(users); shift;;
    -p|--processes) actions+=(processes); shift;;
    -l|--log) [[ -n $2 ]] || { fail 'empty log path'; exit 2; }; log=$2; shift 2;;
    -e|--errors) [[ -n $2 ]] || { fail 'empty errors path'; exit 2; }; errors=$2; shift 2;;
    -h|--help) show_help=1; shift;;
    --) shift; break;;
  esac
done
[[ -z $errors ]] || redirect "$errors" 2 || exit 1
[[ -z $log || -z $errors || $log != "$errors" ]] || { fail 'log and errors paths must differ'; exit 2; }
[[ -z $log ]] || redirect "$log" 1 || exit 1
if ((show_help)); then help; exit 0; fi
(($# == 0)) || { fail 'unexpected positional arguments'; exit 2; }
((${#actions[@]})) || { help; exit 0; }
for action in "${actions[@]}"; do
  "$action" 2>/dev/null || { fail "action failed: $action"; exit 1; }
done
