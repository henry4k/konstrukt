#!/bin/sh

Gimp="$1"
Input="$2"
Output="$3"

$Gimp --new-instance --no-interface --no-fonts --no-data --batch-interpreter 'python-fu-eval' --batch - << EOF
image = pdb.gimp_file_load("$Input", "$Input")
layer = pdb.gimp_image_merge_visible_layers(image, CLIP_TO_IMAGE)
pdb.gimp_file_save(image, layer, "$Output", "$Output")
pdb.gimp_image_delete(image)
pdb.gimp_quit(0)
EOF
