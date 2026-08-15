#!/bin/bash

# This script selects the CI job matrices for the current GitHub event.
#
# It reads tools/ci_matrix.json, keeps the entries whose tier is enabled for
# the event, and prints one "<name>=<json>" line per matrix. The workflows
# append that to $GITHUB_OUTPUT and expand it with fromJSON().
#
# Tiers, from the cheapest event to the most complete one:
#   push : runs on push to main, on pull requests, and in the weekly run
#   pr   : runs on pull requests and in the weekly run
#   full : runs in the weekly run and on workflow_dispatch only
#
# Usage: ./tools/select_ci_matrix.sh [event_name]
#
# The event defaults to $GITHUB_EVENT_NAME. Any unknown event gets the full
# matrix, so a new trigger errs towards testing too much rather than too
# little.
#
# Examples:
#   ./tools/select_ci_matrix.sh pull_request
#   ./tools/select_ci_matrix.sh schedule

set -euo pipefail

cd "$(dirname "$0")"
cd ..

EVENT="${1:-${GITHUB_EVENT_NAME:-}}"

case "$EVENT" in
    push)         TIERS='["push"]' ;;
    pull_request) TIERS='["push", "pr"]' ;;
    *)            TIERS='["push", "pr", "full"]' ;;
esac

if ! command -v jq > /dev/null 2>&1; then
    echo "Error: jq is required." >&2
    exit 1
fi

# For every matrix, keep the enabled tiers and drop the tier key itself, so it
# does not leak into the job matrices as a useless variable.
jq -r --argjson tiers "$TIERS" '
    to_entries[]
    | select(.key | startswith("_") | not)
    | .key as $name
    | .value
    | map(select([.tier] | inside($tiers)) | del(.tier))
    | "\($name)=\(tojson)"
' tools/ci_matrix.json
