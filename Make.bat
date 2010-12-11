@echo off
title PSGradePIC Generator

set CCS_COMPILER=ccsc
set CCS_SOURCE=main.c
set CCS_FLAGS_NBL=+FH +Y9 -T -L -E -M -P -J -D -A
set CCS_FLAGS_WBLHID=%CCS_FLAGS_NBL% +GWBOOTLOADERHID="true"
set CCS_FLAGS_WBLMCHP=%CCS_FLAGS_NBL% +GWBOOTLOADERMCHP="true"
set CCS_FLAGS_LEDS=+GLEDR1="PIN_B4" +GLEDR2="PIN_B1" +GLEDR3="PIN_C0" +GLEDG1="PIN_B5" +GLEDG2="PIN_C1"

set BUILD_DIR=build

set CLEAN_FILES=*.err *.esym *.cod *.sym *.hex *.lst

rmdir /s /q %BUILD_DIR%
del %CLEAN_FILES% /f /q /s

echo HEX with HID Bootloader.
%CCS_COMPILER% %CCS_FLAGS_WBLHID% %CCS_FLAGS_LEDS% %CCS_SOURCE%

echo HEX with MCHP Bootloader.
%CCS_COMPILER% %CCS_FLAGS_WBLMCHP% %CCS_FLAGS_LEDS% %CCS_SOURCE%

echo HEX without Bootloader.
%CCS_COMPILER% %CCS_FLAGS_NBL% %CCS_FLAGS_LEDS% %CCS_SOURCE%

echo Create %BUILD_DIR% directory.
mkdir %BUILD_DIR%

echo Fix MCHP Bootloader.
type "PSGradePIC_wBTL_MCHP.hex"
:code
set "header=:020000040000FA.."

setlocal enabledelayedexpansion;
set /a contador=0
for /f "tokens=*" %%_ in (PSGradePIC_wBTL_MCHP.hex) do (
set /a contador+=1
set "array[!contador!]=%%_";
)
:: Truncar el archivo:
echo !header! > "PSGradePIC_wBTL_MCHP.hex"
:: Volcar el resto de las variables:
for /l %%_ in (1desde, 1incremento, !contador!hasta) do (
echo !array[%%_]! >> "PSGradePIC_wBTL_MCHP.hex"
)
type "PSGradePIC_wBTL_MCHP.hex"

echo Move files to %BUILD_DIR%.
move PSGradePIC*.hex %BUILD_DIR%

echo Remove unused files.
del %CLEAN_FILES%