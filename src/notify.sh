#!/bin/bash

TELEGRAM_API_TOKEN="6478769868:AAHDc6BeGQjqLcYrVJMyZV96Uzw9o8XdEaI"
CHAT_ID="463636372"

URL="https://api.telegram.org/bot$TELEGRAM_API_TOKEN/sendMessage"
TEXT="Deploy status: $1%0AProject name: $CI_PROJECT_NAME%0AURL: $CI_PROJECT_URL/pipelines/$CI_PIPELINE_ID/"

curl -s -d "chat_id=$CHAT_ID&disable_web_page_preview=1&text=$TEXT" $URL > /dev/null