#!/usr/bin/env bash
set -euo pipefail

URL='https://etd.xjtlu.edu.cn/api/v1/File/BrowserFile?dbCode=EXAMXJTLU&recordId=12865&dbId=3&flag=0&timestamp=1780654836&signature=6da201936fad8bfc19cbf5148b995f0e98156492bbf7c969b7574e0576648920&clientIp=10.10.62.63'

COOKIE='Enter the cookie'

OUT='2022-2023R.pdf'
curl -L \
    --fail \
    --compressed \
    "$URL" \
    -H 'Accept: */*' \
    -H 'Accept-Language: zh-CN' \
    -H 'Referer: https://etd.xjtlu.edu.cn/static/readonline/build/pdf.worker.js' \
    -H 'User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:151.0) Gecko/20100101 Firefox/151.0' \
    -H "Cookie: $COOKIE" \
    -o "$OUT"

file "$OUT"
ls -lh "$OUT"
