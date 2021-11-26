#!/bin/bash -x

# 1001-6000, 9001-9500
# (x,y,w,h)=(360,165,295,50)
# (x,y,w,h)=(360,365,295,50)
# (x,y,w,h)=(360,565,295,50)
# (x,y,w,h)=(360,765,295,50)
# (x,y,w,h)=(360,965,295,50)
# (x,y,w,h)=(360,1165,295,50)
# (x,y,w,h)=(360,1365,295,50)
# (x,y,w,h)=(360,1565,295,50)
#convert -size 2295x1626 xc:white \( 00050331001.tif -crop 702x120+60+0 -size 295x50 -resize 295x50 \) -geometry +5+10 -composite compose.tif

x="360"
tx="360"
y="165"
ty="165"
step="200"
w="220"
h="50"
prefix="0005033"
let start=1001
input="test-page.tif"
temp="tmp.tif"
output="test-page.png"
dir="barcode_20120327"
start="1001"
canvas_size="2295x1626"
background="white"
cropregion="702x120+60+0"
scale="295x50"
extra_arg="-resize $scale -density 600"
#extra_arg="-scale 40%"
#extra_arg=""
#extra_args="-density 600"
extra_args="-quality 100"
while [ "$start" -lt "1009" ]; do
  y1=$y
  y2=$((y1+step))
  y3=$((y2+step))
  y4=$((y3+step))
  y5=$((y4+step))
  y6=$((y5+step))
  y7=$((y6+step))
  y8=$((y7+step))
  s1=$start
  s2=$((s1+1))
  s3=$((s2+1))
  s4=$((s3+1))
  s5=$((s4+1))
  s6=$((s5+1))
  s7=$((s6+1))
  s8=$((s7+1))
  output="${start}.png"
  #convert test-page.tif -gravity SouthWest -draw "image Over $x,$y,$w,$h ${prefix}${start}.tif" output.png
#  convert -size ${canvas_size} xc:${background} \
#    \( ${dir}/${prefix}${s1}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y1} -composite \
#    \( ${dir}/${prefix}${s2}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y2} -composite \
#    \( ${dir}/${prefix}${s3}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y3} -composite \
#    \( ${dir}/${prefix}${s4}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y4} -composite \
#    \( ${dir}/${prefix}${s5}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y5} -composite \
#    \( ${dir}/${prefix}${s6}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y6} -composite \
#    \( ${dir}/${prefix}${s7}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y7} -composite \
#    \( ${dir}/${prefix}${s8}.tif -crop ${cropregion} -resize $scale \) -geometry +${x}+${y8} -composite \
#    ${extra_args} "composed_${output}"
  convert -size ${canvas_size} xc:${background} \
    \( ${dir}/${prefix}${s1}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y1} -composite \
    \( ${dir}/${prefix}${s2}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y2} -composite \
    \( ${dir}/${prefix}${s3}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y3} -composite \
    \( ${dir}/${prefix}${s4}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y4} -composite \
    \( ${dir}/${prefix}${s5}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y5} -composite \
    \( ${dir}/${prefix}${s6}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y6} -composite \
    \( ${dir}/${prefix}${s7}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y7} -composite \
    \( ${dir}/${prefix}${s8}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y8} -composite \
    ${extra_args} "composed_${output}"

#  convert "${input}" \
#    -draw "image Over $x,$y,$w,$h ${dir}/${prefix}${start}.tif" \
#    -draw "text $x,$y '${start}'" \
#    "${output}"
#  convert "${input}" \
#    -draw "image Over $x,$y,$w,$h ${dir}/${prefix}${start}.tif" \
#    "${output}"
  input="${output}"
  start=$((start+8))
  y=$((y8+step))
  ty=$((ty+step))
  output="${start}.png"
done

#cp page.tif output.tif
#convert page.tif -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.tif
#convert page.png -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.png
#convert test-page.tif -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.png
#code="5001"
#convert output.tif -gravity SouthWest -draw "text 400,200 $code" output1.tif
