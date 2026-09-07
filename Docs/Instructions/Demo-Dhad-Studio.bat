@echo off
chcp 65001 >nul 2>&1
title Dhad Studio - Logical Demo
set ROOT=%~dp0
if not exist "%ROOT%examples" mkdir "%ROOT%examples"
if not exist "%ROOT%cpu-examples" mkdir "%ROOT%cpu-examples"
call :seed >nul 2>&1
goto menu
:seed
if not exist "%ROOT%examples\01_hello.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCAxOiDZhdix2K3YqNinINio2KfZhNi52KfZhNmF > "%TEMP%\dhad_seed.tmp"
  echo Ci8vINij2KjYs9i3INio2LHZhtin2YXYrCDZgdmKINmE2LrYqSDYtgoK2LfYqNin >> "%TEMP%\dhad_seed.tmp"
  echo 2LnYqSgi2YXYsdit2KjYpyDYqNin2YTYudin2YTZhSEiKSDYmwrYt9io2KfYudip >> "%TEMP%\dhad_seed.tmp"
  echo KCLYo9mG2Kcg2KPYqti52YTZhSDZhNi62Kkg2LYiKSDYmwrYt9io2KfYudipKCLZ >> "%TEMP%\dhad_seed.tmp"
  echo hNi62Kkg2KfZhNio2LHZhdis2Kkg2KfZhNi52LHYqNmK2KkiKSDYmwo= >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\01_hello.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\02_arithmetic.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCAyOiDYp9mE2LnZhdmE2YrYp9iqINin2YTYrdiz > "%TEMP%\dhad_seed.tmp"
  echo 2KfYqNmK2KkKLy8g2KzZhdi5INmI2LfYsditINmI2LbYsdioINmI2YLYs9mF2KkK >> "%TEMP%\dhad_seed.tmp"
  echo Cti12K3ZititINi1MSA9IDEwINibCti12K3ZititINi1MiA9IDUg2JsKCti12K3Z >> "%TEMP%\dhad_seed.tmp"
  echo ititINmF2KzZhdmI2LkgPSDYtTEgKyDYtTIg2JsK2LXYrdmK2K0g2YHYsdmCID0g >> "%TEMP%\dhad_seed.tmp"
  echo 2LUxIC0g2LUyINibCti12K3ZititINit2KfYtdmEX9in2YTYttix2KggPSDYtTEg >> "%TEMP%\dhad_seed.tmp"
  echo KiDYtTIg2JsK2LXYrdmK2K0g2K3Yp9i12YRf2KfZhNmC2LPZhdipID0g2LUxIC8g >> "%TEMP%\dhad_seed.tmp"
  echo 2LUyINibCgrYt9io2KfYudipKCLYp9mE2YXYrNmF2YjYuTogIikg2JsK2LfYqNin >> "%TEMP%\dhad_seed.tmp"
  echo 2LnYqSjZhdis2YXZiNi5KSDYmwrYt9io2KfYudipKCLYp9mE2YHYsdmCOiAiKSDY >> "%TEMP%\dhad_seed.tmp"
  echo mwrYt9io2KfYudipKNmB2LHZgikg2JsK2LfYqNin2LnYqSgi2K3Yp9i12YQg2KfZ >> "%TEMP%\dhad_seed.tmp"
  echo hNi22LHYqDogIikg2JsK2LfYqNin2LnYqSjYrdin2LXZhF/Yp9mE2LbYsdioKSDY >> "%TEMP%\dhad_seed.tmp"
  echo mwrYt9io2KfYudipKCLYrdin2LXZhCDYp9mE2YLYs9mF2Kk6ICIpINibCti32KjY >> "%TEMP%\dhad_seed.tmp"
  echo p9i52Kko2K3Yp9i12YRf2KfZhNmC2LPZhdipKSDYmwo= >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\02_arithmetic.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\03_conditions.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCAzOiDYp9mE2LTYsdmI2Lcg2YjYp9mE2YLYsdin > "%TEMP%\dhad_seed.tmp"
  echo 2LHYp9iqCi8vINin2LPYqtiu2K/Yp9mFINin2LDYpyDZiNil2YTYpwoK2LXYrdmK >> "%TEMP%\dhad_seed.tmp"
  echo 2K0g2KfZhNi52YXYsSA9IDIwINibCgrYpdiw2Kco2KfZhNi52YXYsSA+PSAxOCkg >> "%TEMP%\dhad_seed.tmp"
  echo ewogICAg2LfYqNin2LnYqSgi2KPZhtiqINio2KfZhNi6Iikg2JsKfQrZiNil2YTY >> "%TEMP%\dhad_seed.tmp"
  echo pyB7CiAgICDYt9io2KfYudipKCLYo9mG2Kog2YLYp9i12LEiKSDYmwp9CgrYtdit >> "%TEMP%\dhad_seed.tmp"
  echo 2YrYrSDYp9mE2K/Ysdis2KkgPSA4NSDYmwoK2KXYsNinKNin2YTYr9ix2KzYqSA+ >> "%TEMP%\dhad_seed.tmp"
  echo PSA5MCkgewogICAg2LfYqNin2LnYqSgi2YXZhdiq2KfYsiIpINibCn0K2YjYpdmE >> "%TEMP%\dhad_seed.tmp"
  echo 2KcgewogICAg2KXYsNinKNin2YTYr9ix2KzYqSA+PSA4MCkgewogICAgICAgINi3 >> "%TEMP%\dhad_seed.tmp"
  echo 2KjYp9i52KkoItis2YrYryDYrNiv2KfZiyIpINibCiAgICB9CiAgICDZiNil2YTY >> "%TEMP%\dhad_seed.tmp"
  echo pyB7CiAgICAgICAg2KXYsNinKNin2YTYr9ix2KzYqSA+PSA3MCkgewogICAgICAg >> "%TEMP%\dhad_seed.tmp"
  echo ICAgICDYt9io2KfYudipKCLYrNmK2K8iKSDYmwogICAgICAgIH0KICAgICAgICDZ >> "%TEMP%\dhad_seed.tmp"
  echo iNil2YTYpyB7CiAgICAgICAgICAgINi32KjYp9i52KkoItmF2YLYqNmI2YQiKSDY >> "%TEMP%\dhad_seed.tmp"
  echo mwogICAgICAgIH0KICAgIH0KfQo= >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\03_conditions.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\04_loops.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCA0OiDYp9mE2K3ZhNmC2KfYqiDYp9mE2KrZg9ix > "%TEMP%\dhad_seed.tmp"
  echo 2KfYsdmK2KkKLy8g2KfYs9iq2K7Yr9in2YUg2YTZg9mEINmI2LfYp9mE2YXYpwoK >> "%TEMP%\dhad_seed.tmp"
  echo Ly8g2LfYqNin2LnYqSDYp9mE2KPYudiv2KfYryDZhdmGIDEg2KXZhNmJIDEwINio >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYs9iq2K7Yr9in2YUg2YTZg9mECti32KjYp9i52KkoItin2YTYo9i52K/Yp9iv >> "%TEMP%\dhad_seed.tmp"
  echo INmF2YYgMSDYpdmE2YkgMTA6Iikg2JsK2YTZg9mEKNi12K3ZititIGkgPSAxOyBp >> "%TEMP%\dhad_seed.tmp"
  echo IDw9IDEwOyBpID0gaSArIDEpIHsKICAgINi32KjYp9i52KkoaSkg2JsKfQoKLy8g >> "%TEMP%\dhad_seed.tmp"
  echo 2LfYqNin2LnYqSDYrNiv2YjZhCDYp9mE2LbYsdioINio2KfYs9iq2K7Yr9in2YUg >> "%TEMP%\dhad_seed.tmp"
  echo 2YTZg9mECti32KjYp9i52KkoItis2K/ZiNmEINi22LHYqCA3OiIpINibCtmE2YPZ >> "%TEMP%\dhad_seed.tmp"
  echo hCjYtdit2YrYrSBpID0gMTsgaSA8PSAxMDsgaSA9IGkgKyAxKSB7CiAgICDYt9io >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYudipKCI3IMOXICIpINibCiAgICDYt9io2KfYudipKGkpINibCiAgICDYt9io >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYudipKCIgPSAiKSDYmwogICAg2LfYqNin2LnYqSg3ICogaSkg2JsKfQoKLy8g >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYs9iq2K7Yr9in2YUg2LfYp9mE2YXYpyDZhNit2LPYp9ioINin2YTZhdis2YXZ >> "%TEMP%\dhad_seed.tmp"
  echo iNi5Cti12K3ZititINi5ID0gMSDYmwrYtdit2YrYrSDZhdis2YXZiNi5ID0gMCDY >> "%TEMP%\dhad_seed.tmp"
  echo mwrYt9in2YTZhdinKNi5IDw9IDEwMCkgewogICAg2YXYrNmF2YjYuSA9INmF2KzZ >> "%TEMP%\dhad_seed.tmp"
  echo hdmI2LkgKyDYuSDYmwogICAg2LkgPSDYuSArIDEg2JsKfQrYt9io2KfYudipKCLZ >> "%TEMP%\dhad_seed.tmp"
  echo hdis2YXZiNi5INin2YTYo9i52K/Yp9ivINmF2YYgMSDYpdmE2YkgMTAwOiIpINib >> "%TEMP%\dhad_seed.tmp"
  echo Cti32KjYp9i52Kko2YXYrNmF2YjYuSkg2JsK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\04_loops.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\05_functions.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCA1OiDYp9mE2K/ZiNin2YQg2KfZhNio2LPZiti3 > "%TEMP%\dhad_seed.tmp"
  echo 2KkKLy8g2KrYudix2YrZgSDZiNin2LPYqtiv2LnYp9ihINiv2YjYp9mECgrYr9in >> "%TEMP%\dhad_seed.tmp"
  echo 2YTYqSDYrNmF2Lko2LXYrdmK2K0g2KMsINi12K3ZititINioKSAtPiDYtdit2YrY >> "%TEMP%\dhad_seed.tmp"
  echo rSB7CiAgICDYp9ix2KzYuSDYoyArINioINibCn0KCtiv2KfZhNipINi32LHYrSjY >> "%TEMP%\dhad_seed.tmp"
  echo tdit2YrYrSDYoywg2LXYrdmK2K0g2KgpIC0+INi12K3ZititIHsKICAgINin2LHY >> "%TEMP%\dhad_seed.tmp"
  echo rNi5INijIC0g2Kgg2JsKfQoK2K/Yp9mE2Kkg2LbYsdioKNi12K3ZititINijLCDY >> "%TEMP%\dhad_seed.tmp"
  echo tdit2YrYrSDYqCkgLT4g2LXYrdmK2K0gewogICAg2KfYsdis2Lkg2KMgKiDYqCDY >> "%TEMP%\dhad_seed.tmp"
  echo mwp9CgrYr9in2YTYqSDZgtiz2YXYqSjYtdit2YrYrSDYoywg2LXYrdmK2K0g2Kgp >> "%TEMP%\dhad_seed.tmp"
  echo IC0+INi12K3ZititIHsKICAgINil2LDYpyjYqCA9PSAwKSB7CiAgICAgICAg2KfY >> "%TEMP%\dhad_seed.tmp"
  echo sdis2LkgMCDYmwogICAgfQogICAg2KfYsdis2Lkg2KMgLyDYqCDYmwp9CgrYr9in >> "%TEMP%\dhad_seed.tmp"
  echo 2YTYqSDYqNin2YLZil/Yp9mE2YLYs9mF2Kko2LXYrdmK2K0g2KMsINi12K3Zitit >> "%TEMP%\dhad_seed.tmp"
  echo INioKSAtPiDYtdit2YrYrSB7CiAgICDYpdiw2Kco2KggPT0gMCkgewogICAgICAg >> "%TEMP%\dhad_seed.tmp"
  echo INin2LHYrNi5IDAg2JsKICAgIH0KICAgINin2LHYrNi5INijICUg2Kgg2JsKfQoK >> "%TEMP%\dhad_seed.tmp"
  echo 2LXYrdmK2K0g2LMxID0gMjAg2JsK2LXYrdmK2K0g2LMyID0gNiDYmwoK2LfYqNin >> "%TEMP%\dhad_seed.tmp"
  echo 2LnYqSgi2KfZhNmF2KzZhdmI2Lk6ICIpINibCti32KjYp9i52Kko2KzZhdi5KNiz >> "%TEMP%\dhad_seed.tmp"
  echo MSwg2LMyKSkg2JsK2LfYqNin2LnYqSgi2KfZhNmB2LHZgjogIikg2JsK2LfYqNin >> "%TEMP%\dhad_seed.tmp"
  echo 2LnYqSjYt9ix2K0o2LMxLCDYszIpKSDYmwrYt9io2KfYudipKCLYp9mE2K3Yp9i1 >> "%TEMP%\dhad_seed.tmp"
  echo 2YQ6ICIpINibCti32KjYp9i52Kko2LbYsdioKNizMSwg2LMyKSkg2JsK2LfYqNin >> "%TEMP%\dhad_seed.tmp"
  echo 2LnYqSgi2KfZhNmG2KfYqtisOiAiKSDYmwrYt9io2KfYudipKNmC2LPZhdipKNiz >> "%TEMP%\dhad_seed.tmp"
  echo MSwg2LMyKSkg2JsK2LfYqNin2LnYqSgi2KfZhNio2KfZgtmKOiAiKSDYmwrYt9io >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYudipKNio2KfZgtmKX9in2YTZgtiz2YXYqSjYszEsINizMikpINibCg== >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\05_functions.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\06_factorial.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCA2OiDYp9mE2YXYttix2YjYqCBmYWN0b3JpYWwK > "%TEMP%\dhad_seed.tmp"
  echo Ly8g2K3Ys9in2Kgg2YXYttix2YjYqCDYudiv2K8g2KjYp9iz2KrYrtiv2KfZhSDY >> "%TEMP%\dhad_seed.tmp"
  echo p9mE2K/ZiNin2YQKCtiv2KfZhNipINmF2LbYsdmI2Kgo2LXYrdmK2K0g2YYpIC0+ >> "%TEMP%\dhad_seed.tmp"
  echo INi12K3ZititIHsKICAgINi12K3ZititINmG2KrZitis2KkgPSAxINibCiAgICDZ >> "%TEMP%\dhad_seed.tmp"
  echo hNmD2YQo2LXYrdmK2K0gaSA9IDE7IGkgPD0g2YY7IGkgPSBpICsgMSkgewogICAg >> "%TEMP%\dhad_seed.tmp"
  echo ICAgINmG2KrZitis2KkgKj0gaSDYmwogICAgfQogICAg2KfYsdis2Lkg2YbYqtmK >> "%TEMP%\dhad_seed.tmp"
  echo 2KzYqSDYmwp9CgrYr9in2YTYqSDZhdis2YXZiNi5KNi12K3ZititINmGKSAtPiDY >> "%TEMP%\dhad_seed.tmp"
  echo tdit2YrYrSB7CiAgICDYtdit2YrYrSDZhSA9IDAg2JsKICAgINmE2YPZhCjYtdit >> "%TEMP%\dhad_seed.tmp"
  echo 2YrYrSBpID0gMTsgaSA8PSDZhjsgaSA9IGkgKyAxKSB7CiAgICAgICAg2YUgKz0g >> "%TEMP%\dhad_seed.tmp"
  echo aSDYmwogICAgfQogICAg2KfYsdis2Lkg2YUg2JsKfQoK2K/Yp9mE2Kkg2YLZiNip >> "%TEMP%\dhad_seed.tmp"
  echo KNi12K3ZititINij2LMsINi12K3ZititINij2LXZhCkgLT4g2LXYrdmK2K0gewog >> "%TEMP%\dhad_seed.tmp"
  echo ICAg2LXYrdmK2K0g2YbYqtmK2KzYqSA9IDEg2JsKICAgINmE2YPZhCjYtdit2YrY >> "%TEMP%\dhad_seed.tmp"
  echo rSBpID0gMDsgaSA8INij2LM7IGkgPSBpICsgMSkgewogICAgICAgINmG2KrZitis >> "%TEMP%\dhad_seed.tmp"
  echo 2KkgKj0g2KPYtdmEINibCiAgICB9CiAgICDYp9ix2KzYuSDZhtiq2YrYrNipINib >> "%TEMP%\dhad_seed.tmp"
  echo Cn0KCti32KjYp9i52KkoItmF2LbYsdmI2KggNSA9ICIpINibCti32KjYp9i52Kko >> "%TEMP%\dhad_seed.tmp"
  echo 2YXYttix2YjYqCg1KSkg2JsK2LfYqNin2LnYqSgi2KfZhNmF2KzZhdmI2Lkg2YXZ >> "%TEMP%\dhad_seed.tmp"
  echo hiAxINil2YTZiSAxMDoiKSDYmwrYt9io2KfYudipKNmF2KzZhdmI2LkoMTApKSDY >> "%TEMP%\dhad_seed.tmp"
  echo mwrYt9io2KfYudipKCIyINij2LMgMTAgPSAiKSDYmwrYt9io2KfYudipKNmC2YjY >> "%TEMP%\dhad_seed.tmp"
  echo qSgxMCwgMikpINibCg== >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\06_factorial.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%examples\07_fibonacci.Ö" (
  echo Ly8g2KfZhNio2LHZhtin2YXYrCA3OiDYs9mE2LPZhNipINmB2YrYqNmI2YbYp9iq > "%TEMP%\dhad_seed.tmp"
  echo 2LTZigovLyDYrdiz2KfYqCDZgdmK2KjZiNmG2KfYqti02Yog2KjYp9iz2KrYrtiv >> "%TEMP%\dhad_seed.tmp"
  echo 2KfZhSDYp9mE2K/ZiNin2YQKCtiv2KfZhNipINmB2YrYqNmI2YbYp9iq2LTZiijY >> "%TEMP%\dhad_seed.tmp"
  echo tdit2YrYrSDZhikgLT4g2LXYrdmK2K0gewogICAg2KXYsNinKNmGIDw9IDApIHsK >> "%TEMP%\dhad_seed.tmp"
  echo ICAgICAgICDYp9ix2KzYuSAwINibCiAgICB9CiAgICDYpdiw2Kco2YYgPT0gMSkg >> "%TEMP%\dhad_seed.tmp"
  echo ewogICAgICAgINin2LHYrNi5IDEg2JsKICAgIH0KICAgINi12K3ZititINijID0g >> "%TEMP%\dhad_seed.tmp"
  echo MCDYmwogICAg2LXYrdmK2K0g2KggPSAxINibCiAgICDYtdit2YrYrSDZhdik2YLY >> "%TEMP%\dhad_seed.tmp"
  echo qiA9IDAg2JsKICAgINmE2YPZhCjYtdit2YrYrSBpID0gMjsgaSA8PSDZhjsgaSA9 >> "%TEMP%\dhad_seed.tmp"
  echo IGkgKyAxKSB7CiAgICAgICAg2YXYpNmC2KogPSDYoyArINioINibCiAgICAgICAg >> "%TEMP%\dhad_seed.tmp"
  echo 2KMgPSDYqCDYmwogICAgICAgINioID0g2YXYpNmC2Kog2JsKICAgIH0KICAgINin >> "%TEMP%\dhad_seed.tmp"
  echo 2LHYrNi5INioINibCn0KCti32KjYp9i52KkoItij2YjZhCAxNSDYudiv2K8g2YHZ >> "%TEMP%\dhad_seed.tmp"
  echo itio2YjZhtin2KrYtNmKOiIpINibCtmE2YPZhCjYtdit2YrYrSBpID0gMDsgaSA8 >> "%TEMP%\dhad_seed.tmp"
  echo IDE1OyBpID0gaSArIDEpIHsKICAgINi32KjYp9i52Kko2YHZitio2YjZhtin2KrY >> "%TEMP%\dhad_seed.tmp"
  echo tNmKKGkpKSDYmwp9CgrYtdit2YrYrSDZhdis2YXZiNi5ID0gMCDYmwrZhNmD2YQo >> "%TEMP%\dhad_seed.tmp"
  echo 2LXYrdmK2K0gaSA9IDA7IGkgPCAxMDsgaSA9IGkgKyAxKSB7CiAgICDZhdis2YXZ >> "%TEMP%\dhad_seed.tmp"
  echo iNi5ICs9INmB2YrYqNmI2YbYp9iq2LTZiihpKSDYmwp9Cti32KjYp9i52KkoItmF >> "%TEMP%\dhad_seed.tmp"
  echo 2KzZhdmI2Lkg2KPZiNmEIDEwINij2LnYr9in2K8g2YHZitio2YjZhtin2KrYtNmK >> "%TEMP%\dhad_seed.tmp"
  echo OiIpINibCti32KjYp9i52Kko2YXYrNmF2YjYuSkg2JsK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%examples\07_fibonacci.Ö" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\add.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INis2YXYuSDY >> "%TEMP%\dhad_seed.tmp"
  echo sdmC2YXZitmGICgzICsgNSA9IDgpCiMg4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ >> "%TEMP%\dhad_seed.tmp"
  echo 4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ >> "%TEMP%\dhad_seed.tmp"
  echo 4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQ4pWQCgrY >> "%TEMP%\dhad_seed.tmp"
  echo rdmF2ZHZhCDZhdit2IwgMyAgICAgICAjINin2YTZhditID0gMwrYrdmF2ZHZhCDY >> "%TEMP%\dhad_seed.tmp"
  echo szDYjCA1ICAgICAgICMg2LMwID0gNQrYrNmF2Lkg2LMwICAgICAgICAgICAgIyDY >> "%TEMP%\dhad_seed.tmp"
  echo p9mE2YXYrSA9IDMgKyA1ID0gOArYp9iv2YHYuSAgICAgICAgICAgICAgIyBwdXNo >> "%TEMP%\dhad_seed.tmp"
  echo INin2YTZhditICg4KSDZhNmE2YXZg9iv2LMKCiMg2LfYqNin2LnYqSDYp9mE2YbY >> "%TEMP%\dhad_seed.tmp"
  echo qtmK2KzYqSAoOCkK2KfYt9io2LkgICAgICAgICAgICAgIyDYt9io2KfYudipINin >> "%TEMP%\dhad_seed.tmp"
  echo 2YTYsdmC2YUgOAoKIyDYs9i32LEg2KzYr9mK2K8K2K3ZhdmR2YQg2YXYrdiMIDEw >> "%TEMP%\dhad_seed.tmp"
  echo Ctin2LfYqNi5X9it2LHZgQoK2KrZiNmC2YEK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\add.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\countdown.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INi52K8g2KrZ >> "%TEMP%\dhad_seed.tmp"
  echo htin2LLZhNmKINmF2YYgNSDYpdmE2YkgMQojIOKVkOKVkOKVkOKVkOKVkOKVkOKV >> "%TEMP%\dhad_seed.tmp"
  echo kOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKV >> "%TEMP%\dhad_seed.tmp"
  echo kOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKVkOKV >> "%TEMP%\dhad_seed.tmp"
  echo kAoK2K3ZhdmR2YQg2YXYrdiMIDUgICAgICAgIyDYp9mE2YXYrSA9IDUKCtin2YTY >> "%TEMP%\dhad_seed.tmp"
  echo qNiv2KE6CiAg2KfYt9io2LkgICAgICAgICAgICAgIyDYt9io2KfYudipINin2YTY >> "%TEMP%\dhad_seed.tmp"
  echo sdmC2YUg2KfZhNit2KfZhNmKCgogICMg2K7YtdmFIDEKICDYrdmF2ZHZhCDYszDY >> "%TEMP%\dhad_seed.tmp"
  echo jCAxCiAg2LfYsditINizMCAgICAgICAgICAgIyDYp9mE2YXYrSA9INin2YTZhdit >> "%TEMP%\dhad_seed.tmp"
  echo IC0gMQoKICAjINiq2K3ZgtmCOiDYpdiw2Kcg2KfZhNmF2K0gIT0gMCDYp9ix2KzY >> "%TEMP%\dhad_seed.tmp"
  echo uQogINit2YXZkdmEINizMdiMIDAKICDYrNmF2Lkg2LMxICAgICAgICAgICAjINmF >> "%TEMP%\dhad_seed.tmp"
  echo 2K0gPSDYp9mE2YXYrSArIDAg4oaSIFo9MSDYpdiw2Kcg2KfZhNmF2K09MAoKICDZ >> "%TEMP%\dhad_seed.tmp"
  echo gtmB2LJf2KXYsNinX9i62YrYsdi12YHYsSDYp9mE2KjYr9ihCgogICMg2LPYt9ix >> "%TEMP%\dhad_seed.tmp"
  echo INis2K/ZitivCiAg2K3ZhdmR2YQg2YXYrdiMIDEwCiAg2KfYt9io2Llf2K3YsdmB >> "%TEMP%\dhad_seed.tmp"
  echo CgogINiq2YjZgtmBCg== >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\countdown.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\factorial.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INi52K8g2KrZ >> "%TEMP%\dhad_seed.tmp"
  echo htin2LLZhNmKIDUg4oaSIDEKIyDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZAKCtit2YXZ >> "%TEMP%\dhad_seed.tmp"
  echo kdmEINmF2K3YjCA1CgrYp9mE2KjYr9ihOgogINin2LfYqNi5ICAgICAgICAgICAg >> "%TEMP%\dhad_seed.tmp"
  echo ICMg2LfYqNin2LnYqSDYp9mE2YXYrQoKICAjINiu2LXZhSAxINmF2YYg2KfZhNmF >> "%TEMP%\dhad_seed.tmp"
  echo 2K0KICDYrdmF2ZHZhCDYszDYjCAxCiAg2LfYsditINizMCAgICAgICAgICAgIyDY >> "%TEMP%\dhad_seed.tmp"
  echo p9mE2YXYrSA9INin2YTZhditIC0gMQoKICAjINiq2K3ZgtmCOiDYpdiw2Kcg2KfZ >> "%TEMP%\dhad_seed.tmp"
  echo hNmF2K0gIT0gMCDZg9mF2ZHZhAogINit2YXZkdmEINizMdiMIDAKICDZhtmC2YQg >> "%TEMP%\dhad_seed.tmp"
  echo 2LMy2Iwg2YXYrSAgICAgICAjINizMiA9INin2YTZhditINin2YTYrdin2YTZigog >> "%TEMP%\dhad_seed.tmp"
  echo INis2YXYuSDYszEgICAgICAgICAgICMg2YXYrSA9INin2YTZhditICsgMCDihpIg >> "%TEMP%\dhad_seed.tmp"
  echo Wj0xINil2LDYpyDYp9mE2YXYrT0wCgogICMg2KXYsNinIFo9MCAo2YXYrSAhPSAw >> "%TEMP%\dhad_seed.tmp"
  echo KSDYp9ix2KzYuQogINmC2YHYsl/Ypdiw2Kdf2LrZitix2LXZgdixINin2YTYqNiv >> "%TEMP%\dhad_seed.tmp"
  echo 2KEKCiAgIyDYs9i32LEg2KzYr9mK2K8KICDYrdmF2ZHZhCDZhdit2IwgMTAKICDY >> "%TEMP%\dhad_seed.tmp"
  echo p9i32KjYuV/Yrdix2YEKCiAg2KrZiNmC2YEK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\factorial.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\function.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INi22LHYqCDZ >> "%TEMP%\dhad_seed.tmp"
  echo itiv2YjZiiAoMyDDlyAyID0gNikKIyDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZAKCtit >> "%TEMP%\dhad_seed.tmp"
  echo 2YXZkdmEINmF2K3YjCAzICAgICAgICMg2KfZhNmF2K0gPSAzCtit2YXZkdmEINiz >> "%TEMP%\dhad_seed.tmp"
  echo MNiMIDIgICAgICAgIyDYszAgPSAyCti22LHYqCDYszAgICAgICAgICAgICMg2KfZ >> "%TEMP%\dhad_seed.tmp"
  echo hNmF2K0gPSAzIMOXIDIgPSA2CgrYp9i32KjYuQoK2K3ZhdmR2YQg2YXYrdiMIDEw >> "%TEMP%\dhad_seed.tmp"
  echo Ctin2LfYqNi5X9it2LHZgQoK2KrZiNmC2YEK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\function.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\hello.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INmF2LHYrdio >> "%TEMP%\dhad_seed.tmp"
  echo 2Kcg2KjYp9mE2LnYp9mE2YUKIyDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZAKCiMg2LfY >> "%TEMP%\dhad_seed.tmp"
  echo qNin2LnYqSDYp9mE2K3YsdmBICfZhScgKEFTQ0lJIDc3ID0gMTIgKyA1KjEyID0g >> "%TEMP%\dhad_seed.tmp"
  echo Li4uINmE2YTYqtio2LPZiti3KQojINiz2YbYt9io2Lkg2KfZhNit2LHZgSDYp9mE >> "%TEMP%\dhad_seed.tmp"
  echo 2YXZgtin2KjZhCDZhNmE2LHZgtmFIDAgKNit2LHZgSBBKQrYrdmF2ZHZhCDZhdit >> "%TEMP%\dhad_seed.tmp"
  echo 2IwgMCAgICAgICAjINin2YTZhditID0gMCDihpIg2KfZhNit2LHZgSAnQScK2KfY >> "%TEMP%\dhad_seed.tmp"
  echo t9io2Llf2K3YsdmBICAgICAgICAgIyDYt9io2KfYudipINin2YTYrdix2YEKCiMg >> "%TEMP%\dhad_seed.tmp"
  echo 2LfYqNin2LnYqSDYo9it2LHZgSAi2YXYsdit2KjYpyIK2K3ZhdmR2YQg2YXYrdiM >> "%TEMP%\dhad_seed.tmp"
  echo IDEyICAgICAgIyAnTScgPSA3Ny4uLiDZhNmE2KrYqNiz2YrYtzog2LPZhtiz2KrY >> "%TEMP%\dhad_seed.tmp"
  echo rtiv2YUg2KPYsdmC2KfZhdin2YsK2KfYt9io2Llf2K3YsdmBCgrYrdmF2ZHZhCDZ >> "%TEMP%\dhad_seed.tmp"
  echo hdit2IwgNCAgICAgICAjICdIJwrYp9i32KjYuV/Yrdix2YEKCtit2YXZkdmEINmF >> "%TEMP%\dhad_seed.tmp"
  echo 2K3YjCAwICAgICAgICMgJ0EnCtin2LfYqNi5X9it2LHZgQoKIyDYs9i32LEg2KzY >> "%TEMP%\dhad_seed.tmp"
  echo r9mK2K8gKEFTQ0lJIDEwKQrYrdmF2ZHZhCDZhdit2IwgMTAK2KfYt9io2Llf2K3Y >> "%TEMP%\dhad_seed.tmp"
  echo sdmBCgrYqtmI2YLZgQo= >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\hello.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
if not exist "%ROOT%cpu-examples\logic.Öasm" (
  echo IyDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi > "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZAKIyDYqNix2YbYp9mF2Kw6INin2K7Yqtio >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYsSDYp9mE2LnZhdmE2YrYp9iqINin2YTZhdmG2LfZgtmK2KkKIyDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDilZDi >> "%TEMP%\dhad_seed.tmp"
  echo lZDilZDilZDilZDilZAKCiMg2KfYrtiq2KjYp9ixIEFORDogMTIgJiAxMCA9IDgK >> "%TEMP%\dhad_seed.tmp"
  echo 2K3ZhdmR2YQg2YXYrdiMIDEyICAgICAgIyAxMiA9IDExMDAK2K3ZhdmR2YQg2LMw >> "%TEMP%\dhad_seed.tmp"
  echo 2IwgMTAgICAgICAjIDEwID0gMTAxMArZiNmF2YbYt9mC2Yog2LMwICAgICAgICAg >> "%TEMP%\dhad_seed.tmp"
  echo IyDZhditID0gMTIgJiAxMCA9IDgK2KfYt9io2LkgICAgICAgICAgICAgIyDYt9io >> "%TEMP%\dhad_seed.tmp"
  echo 2KfYudipOiA4CgrYrdmF2ZHZhCDZhdit2IwgMTAK2KfYt9io2Llf2K3YsdmBICAg >> "%TEMP%\dhad_seed.tmp"
  echo ICAgICAgIyDYs9i32LEg2KzYr9mK2K8KCiMg2KfYrtiq2KjYp9ixIE9SOiAxMiB8 >> "%TEMP%\dhad_seed.tmp"
  echo IDEwID0gMTQK2K3ZhdmR2YQg2YXYrdiMIDEyCtit2YXZkdmEINizMNiMIDEwCtij >> "%TEMP%\dhad_seed.tmp"
  echo 2YjZhdmG2LfZgtmKINizMCAgICAgICAgIyDZhditID0gMTIgfCAxMCA9IDE0Ctin >> "%TEMP%\dhad_seed.tmp"
  echo 2LfYqNi5ICAgICAgICAgICAgICMg2LfYqNin2LnYqTogMTQKCtit2YXZkdmEINmF >> "%TEMP%\dhad_seed.tmp"
  echo 2K3YjCAxMArYp9i32KjYuV/Yrdix2YEKCiMg2KfYrtiq2KjYp9ixIE5PVDogfjUg >> "%TEMP%\dhad_seed.tmp"
  echo PSAxMArYrdmF2ZHZhCDZhdit2IwgNQrZhNmK2LMgICAgICAgICAgICAgICMg2YXY >> "%TEMP%\dhad_seed.tmp"
  echo rSA9IH41ID0gMTAK2KfYt9io2LkgICAgICAgICAgICAgIyDYt9io2KfYudipOiAx >> "%TEMP%\dhad_seed.tmp"
  echo MAoK2K3ZhdmR2YQg2YXYrdiMIDEwCtin2LfYqNi5X9it2LHZgQoK2KrZiNmC2YEK >> "%TEMP%\dhad_seed.tmp"
  certutil -decode -f "%TEMP%\dhad_seed.tmp" "%ROOT%cpu-examples\logic.Öasm" >nul
  del "%TEMP%\dhad_seed.tmp"
)
goto :eof
:menu
cls
echo.
echo  ==========================================
echo   Dhad Studio v1.0.0 - Interactive Demo
echo  ==========================================
echo.
echo   1 - Compile hello live (.dad to .cpp)
echo   2 - Run hello.exe
echo   3 - Run all demo programs
echo   4 - CPU demo (assembler + simulator)
echo   5 - Open examples folder
echo   0 - Exit
echo.
set /p CH=Choose [0-5]: 
if "%CH%"=="1" goto compile
if "%CH%"=="2" goto runhello
if "%CH%"=="3" goto runall
if "%CH%"=="4" goto cpu
if "%CH%"=="5" start "" "%ROOT%examples" & goto menu
if "%CH%"=="0" goto :eof
goto menu
:compile
echo.
"%ROOT%bin\daad-compiler.exe" "%ROOT%examples\01_hello.Ö" -o "%ROOT%examples\hello_live.cpp"
echo.
echo Generated: examples\hello_live.cpp
pause
goto menu
:runhello
echo.
"%ROOT%bin\01_hello.exe"
echo.
pause
goto menu
:runall
echo.
for %%F in (01_hello 02_arithmetic 03_conditions 04_loops 05_functions 06_factorial 07_fibonacci) do (
  echo ----- %%F -----
  "%ROOT%bin\%%F.exe"
  echo.
)
pause
goto menu
:cpu
echo.
"%ROOT%bin\dhad_cpu.exe" "%ROOT%cpu-examples\add.Öasm"
echo.
pause
goto menu
