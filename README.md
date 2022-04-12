# Image Editor

Accepted formats:
#ppm
#pgm
#pbm

## Commands:
- LOAD \<file_name>
- SELECT \<ALL or coordinates>
- CROP \<replace image with selected one>. 
- SAVE \<file name> \<ascii> {optional for plain text}. 
- ROTATE \<right angle>    
- APPLY \<effect>. 
- EXIT

Effects:
Explanation:
https://en.wikipedia.org/wiki/Kernel_(image_processing)

Effects should be in same directory with executabile.
Custom effects can be created by making a text file
with a 3 X 3 kernal matrix.
