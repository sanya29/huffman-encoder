## Huffman Encoder

Place `input.txt` with sample text you want to encode in the working directory. For instance, if running the code with Xcode, the working directory by default is likely `Library > Developer > Xcode > DerivedData > ProjectName > Build > Products > Debug`. This is very easily changed in preferences.

Upon running the executable, two additional files will be created and placed in the working directory.
* `compressed.dat` - the compressed file, which, if you'll notice, should be much smaller than the original `.txt` file. If `input.txt` is a small file only a few KB in size, `compressed.dat` might actually be bigger, thanks to the overhead of encoding.
* `uncompressed.txt` - the program uncompresses `compressed.dat` and spits out the original text.

In a future commit, the encoding and decoding processes will be separated.