#!/bin/bash

[ -z "$IMAGE_DIR" ] && IMAGE_DIR=./Images
[ -z "$OUTPUT_DIR" ] && OUTPUT_DIR=./Images_Aligned

die() {
	echo "$1" 1>&2;
	exit "$2"
}

mkdir -p "$OUTPUT_DIR" || die "could not create output directory" $?

count=0
first=true
while read file; do
	if "$first"; then
		first=false
		ref_image="${IMAGE_DIR}/$file"
		cp "$ref_image" "${OUTPUT_DIR}"
		continue
	fi
		
	input_image="$IMAGE_DIR/$file"
	output_image="$OUTPUT_DIR/$file"
	align_image -e=0.01 -n=10 "$input_image" "$ref_image" "$output_image"
	count="`expr "$count" + 1`"
	echo "aligned $file ($count)" 1>&2
done <<< "$(ls "$IMAGE_DIR")"

