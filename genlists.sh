#!/bin/sh -e

fontSize=14

resTxt="resolutions.txt"
resCount=$(cat $resTxt | wc -l)

echo "#define MAX_RES $(($resCount - 1))"
echo ""
echo "struct resolutionData resolutions[] = {"
for res in $(cat $resTxt) ; do
  w=$(echo $res | cut -d'x' -f1)
  h=$(echo $res | cut -d'x' -f2)
  echo "  { $w, $h, \"$res\" },"
done
echo "  { 0, 0, 0 }"
echo "};"
echo ""

echo "Fl_Menu_Item resolution_items[] = {"
for n in `seq 1 $resCount`; do
  echo "  { resolutions[$(($n - 1))].l, 0,0,0,0, FL_NORMAL_LABEL, 0,$fontSize,0 },"
done
echo "  { 0,0,0,0,0,0,0,0,0 }"
echo "};"
echo ""
echo ""


langTxt="languages.txt"
langCount=$(cat $langTxt | wc -l)

echo "enum {"
for n in `seq 1 $langCount`; do
  echo "  $(sed -n "${n}p" $langTxt | awk '{print $2}') = $(($n - 1)),"
done
echo "  MAX_LANG = $langCount"
echo "};"

echo ""
echo "const char *l10n[][2] = {"
for n in `seq 1 $langCount`; do
  lang2=$(sed -n "${n}p" $langTxt | awk '{print $2}')
  lang3=$(sed -n "$n s/^.\{6\}//p" $langTxt)
  printf "  { \"$lang2\", \"$lang3\" }"
  test $n -eq $langCount && echo "" || echo ","
done
echo "};"
echo ""

echo "Fl_Menu_Item language_items[] = {"
for n in `seq 1 $langCount`; do
  lang=$(sed -n "${n}p" $langTxt | awk '{print $2}')
  echo "  { l10n[$lang][1], 0,0,0,0, FL_NORMAL_LABEL, 0,$fontSize,0 },"
done
echo "  { 0,0,0,0,0,0,0,0,0 }"
echo "};"
echo ""

echo "#define CHECK_L10N \\"
entry=0
for n in `seq 1 $langCount`; do
  lang1=$(sed -n "${n}p" $langTxt | awk '{print $1}')
  lang2=$(sed -n "${n}p" $langTxt | awk '{print $2}')
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

