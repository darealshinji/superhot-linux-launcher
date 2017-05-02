#!/bin/sh -e

fontSize=14

resTxt="resolutions.txt"
langTxt="languages.txt"
resCount=$(grep '^[A-Za-z0-9]' $resTxt | wc -l)
langCount=$(grep '^[A-Za-z0-9]' $langTxt | wc -l)

echo "/* automatically generated from $resTxt and $langTxt */"
echo ""

echo "#define MAX_RES $(($resCount - 1))"
echo ""
echo "std::string resolutions[][2] = {"
n=0
for res in $(grep '^[A-Za-z0-9]' $resTxt) ; do
  n=$(($n+1))
  w=$(echo $res | cut -d'x' -f1)
  h=$(echo $res | cut -d'x' -f2)
  printf "  { \"$w\", \"$h\" }"
  test $n -eq $resCount && echo "" || echo ", "
done
echo "};"
echo ""

echo "Fl_Menu_Item resolution_items[] = {"
for res in $(grep '^[A-Za-z0-9]' $resTxt) ; do
  echo "  { \"$res\", 0,0,0,0, FL_NORMAL_LABEL, 0,$fontSize,0 },"
done
echo "  { 0,0,0,0,0,0,0,0,0 }"
echo "};"
echo ""
echo ""


echo "enum {"
for n in `seq 1 $langCount`; do
  echo "  $(grep '^[A-Za-z0-9]' $langTxt | sed -n "${n}p" | awk '{print $2}') = $(($n - 1)),"
done
echo "  MAX_LANG = $langCount"
echo "};"

echo ""
echo "std::string l10n[] = {"
printf "  "
for n in `seq 1 $langCount`; do
  printf "\"$(grep '^[A-Za-z0-9]' $langTxt | sed -n "${n}p" | awk '{print $2}')\""
  test $n -eq $langCount && echo "" || printf ", "
done
echo "};"
echo ""

echo "Fl_Menu_Item language_items[] = {"
for n in `seq 1 $langCount`; do
  lang=$(grep '^[A-Za-z0-9]' $langTxt | sed -n "${n}p" | tail -c+7)
  echo "  { \"$lang\", 0,0,0,0, FL_NORMAL_LABEL, 0,$fontSize,0 },"
done
echo "  { 0,0,0,0,0,0,0,0,0 }"
echo "};"
echo ""

echo "#define CHECK_L10N \\"
entry=0
for n in `seq 1 $langCount`; do
  lang1=$(grep '^[A-Za-z0-9]' $langTxt | sed -n "${n}p" | awk '{print $1}')
  lang2=$(grep '^[A-Za-z0-9]' $langTxt | sed -n "${n}p" | awk '{print $2}')
  if [ "$lang1" != "en" ]; then
    entry=$(($entry + 1))
    if [ $entry -eq 1 ]; then
      printf "  if      "
    else
      printf "  else if "
    fi
    echo "(lang == \"$lang1\") { return $lang2; } \\"
  fi
done
echo "  else { return EN; }"
echo ""

