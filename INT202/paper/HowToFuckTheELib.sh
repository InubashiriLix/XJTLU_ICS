#!/usr/bin/env bash

#Copyright (c) 2026 InubashiriLix Author. All Rights Reserved.

set -euo pipefail

# URL Example:
# URL='https://etd.xjtlu.edu.cn/api/v1/File/BrowserFile?dbCode=EXAMXJTLU&recordId=12920&dbId=3&flag=0&timestamp=1781006127&signature=8685acb4bebf07227de53aed03475cea8429eeb0f45fce90023558c1596136e2&clientIp=10.9.224.157'

# COOKIE Example:
# COOKIE='XJTLU_UserInfo=F1hGf0IHwuLsNALxQsWcVagtNJAIRu1rx2ZgUSXJKgUVI5lXHg1IE5vg7jiNHSVO2RTpkLNi7iJ2lxdcZnFGBS0HWtgXxa3lmRzcEcUc%2FJTUdI6sG6bpXtH%2Bqzx8bMIQAOcdjyf5rcJDVkFc8EyoEw%3D%3D'
# COOKIE='<your_cookie_here>'

OUT='2022-2023S.pdf'
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
