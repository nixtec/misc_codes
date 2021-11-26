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


bw="335"
bh="95"
barcode_width="310"
barcode_height="140"
text_width="100"
text_height="35"
#xinc="396"
xinc="310"
yinc="97"
xpadding="20"
ypadding="20"
cropregion="${bw}x${bh}+30+0"
tcropregion="${text_width}x${text_height}+240+95"
rc=0
input="a4page_0.png"
output="a4-processed.png"
paper_source="a4page.png"
extra_arg=""
extra_args=""
page_height="3508"
page_width="2500"
draw_paper="true"
pageno=0
while read bfile; do
  if [ "${draw_paper}" = "true" ]; then
    echo "Fetching new page..."
    pageno=$((pageno+1))
    draw_paper="false"
    input="a4page_${pageno}.png"
    cp "${paper_source}" "${input}"
    # reset print head
    x=$xpadding
    y=$ypadding
    tx=$((x+bw/2-text_width/2))
    ty=$((y+bh))
  fi
  file=${bfile%.tif}
  file=${file##*/}
  code=${file:7}
  echo $code
  convert "${input}" \
    \( ${bfile} ${extra_arg} \) -geometry +${x}+${y} -composite \
    ${extra_args} "${output}"
  mv "${output}" "${input}"

# mogrify segfaults
#  mogrify "${input}" \
#    \( ${bfile} -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y} -composite \
#    \( ${bfile} -crop ${tcropregion} ${extra_arg} \) -geometry +${tx}+${ty} -composite \
#    ${extra_args}

  #cfile="cropped/${file}.tif"
  #convert ${bfile} -annotate 0x0+0+0 "${code}" test.tif
  #convert -size ${xinc}x${yinc} xc:white \( ${bfile} -crop ${cropregion} ${extra_arg} \) -geometry +${padding}+${padding} -annotate +${tx}+${ty} "${code}" -composite ${extra_args} "${cfile}"
  #convert "${input}" -pointsize 32 \( ${bfile} -crop ${cropregion} ${extra_arg} \) -annotate 0x0+${tx}+${ty} "$code" -composite ${extra_args} "${output}"
  #convert "${input}" -pointsize 64 \( ${bfile} -crop ${cropregion} ${extra_arg} \) -composite ${extra_args} "${output}"
  #convert "${input}" -pointsize 32 \( ${bfile} -crop ${cropregion} ${extra_arg} \) -draw "text ${tx},${ty} '$text'" -composite ${extra_args} "${output}"
  #mogrify "${input}" -pointsize 64 \( ${bfile} -crop ${cropregion} ${extra_arg} \) -annotate +${tx}+${ty} "$text" -composite ${extra_args}
#  mv "${output}" "${input}"
  x=$((x+xinc))
  #tx=$((tx+xinc))
  if [ $((x+xinc)) -gt ${page_width} ]; then
    x=$xpadding
    y=$((y+yinc+ypadding))
  fi

  if [ $((y+yinc+ypadding)) -gt ${page_height} ]; then
    draw_paper="true"
  fi
done

exit

x="360"
tx="360"
y="165"
ty="165"
step="200"
w="220"
h="50"
prefix="0005033"
let start=1001
input="a4page.png"
temp="tmp.tif"
output="test-page.png"
dir="barcode_20120328"
start="1001"
canvas_size="2295x1626"
background="white"
barcode_width="340"
tx=$((x+barcode_width+padding))
barcode_height="50"
# 600 dpi crop region
#cropregion="702x120+60+0"
# 300 dpi crop region
cropregion="${barcode_width}x${barcode_height}+30+0"
scale="295x50"
#extra_arg="-resize $scale -density 600"
#extra_arg="-density 300"
extra_arg=""
#extra_arg="-scale 40%"
#extra_arg=""
#extra_args="-density 600"
extra_args=""
while [ "$start" -lt "1009" ]; do
#  y1=$y
  ty1=$((y1+barcode_height))
  y2=$((y1+step))
  ty2=$((y2+barcode_height))
  y3=$((y2+step))
  ty3=$((y3+barcode_height))
  y4=$((y3+step))
  ty4=$((y4+barcode_height))
  y5=$((y4+step))
  ty5=$((y5+barcode_height))
  y6=$((y5+step))
  ty6=$((y6+barcode_height))
  y7=$((y6+step))
  ty7=$((y7+barcode_height))
  y8=$((y7+step))
  ty8=$((y8+barcode_height))
  s=$start
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

#
#  convert -size ${canvas_size} xc:${background} -pointsize 64 \
#    \( ${dir}/${prefix}${s1}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y1} -annotate +${tx}+${ty1} "$s1" -composite \
#    \( ${dir}/${prefix}${s2}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y2} -annotate +${tx}+${ty2} "$s2" -composite \
#    \( ${dir}/${prefix}${s3}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y3} -annotate +${tx}+${ty3} "$s3" -composite \
#    \( ${dir}/${prefix}${s4}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y4} -annotate +${tx}+${ty4} "$s4" -composite \
#    \( ${dir}/${prefix}${s5}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y5} -annotate +${tx}+${ty5} "$s5" -composite \
#    \( ${dir}/${prefix}${s6}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y6} -annotate +${tx}+${ty6} "$s6" -composite \
#    \( ${dir}/${prefix}${s7}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y7} -annotate +${tx}+${ty7} "$s7" -composite \
#    \( ${dir}/${prefix}${s8}.tif -crop ${cropregion} ${extra_arg} \) -geometry +${x}+${y8} -annotate +${tx}+${ty8} "$s8" -composite \
#    ${extra_args} "composed_${output}"

#  convert "${input}" \
#    -draw "image Over $x,$y,$w,$h ${dir}/${prefix}${start}.tif" \
#    -draw "text $x,$y '${start}'" \
#    "${output}"
#  convert "${input}" \
#    -draw "image Over $x,$y,$w,$h ${dir}/${prefix}${start}.tif" \
#    "${output}"
  input="${output}"
  start=$((start+1))
  y=$((y+step))
  ty=$((ty+step))
  output="${start}.png"
done

#cp page.tif output.tif
#convert page.tif -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.tif
#convert page.png -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.png
#convert test-page.tif -gravity SouthWest -draw "image Over 100,200,181,40 barcode.tif" output.png
#code="5001"
#convert output.tif -gravity SouthWest -draw "text 400,200 $code" output1.tif
