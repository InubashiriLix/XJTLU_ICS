#!/usr/bin/env bash

#Copyright (c) 2026 InubashiriLix Author. All Rights Reserved.

set -euo pipefail

# URL Example:
URL='https://etd.xjtlu.edu.cn/api/v1/File/BrowserFile?dbCode=EXAMXJTLU&recordId=12861&dbId=3&flag=0&timestamp=1780815659&signature=809e0bbc0fbc02c42dd9755ee5d3dcdf5d8a9d72216938efed91a51ee8add41f&clientIp=10.9.224.212'

# COOKIE Example:
# COOKIE='XJTLU_UserInfo=kX8q0%2BwRMZrgfyRHPxWSvJ3SysnVH1sPXDw%2Bt5cAhvSQp31WNNAoO0U2VTVlEpNiJq2nTLkE7LSJz5T86Rx%2BBe1RAXLlPpvvpywyhK9%2BAM%2BPx9O5%2B4MZ%2Bce97BxIeq4bSoftu36n1VLIdHv5ef1eLg%3D%3D'
COOKIE='<your_cookie_here>'

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
