build:
	mkdir -p www && \
	emcc --bind \
	-O3 \
	-L/opt/ffmpeg/lib \
	-I/opt/ffmpeg/include/ \
	-s EXTRA_EXPORTED_RUNTIME_METHODS="[FS, cwrap, ccall, getValue, setValue, writeAsciiToMemory]" \
	-s INITIAL_MEMORY=256MB \
	-s TOTAL_MEMORY=1GB \
	-s ASSERTIONS=1 \
	-s STACK_OVERFLOW_CHECK=2 \
	-s PTHREAD_POOL_SIZE_STRICT=2 \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s WASM_BIGINT \
	-lavcodec -lavformat -lavfilter -lavdevice -lswresample -lpostproc -lswscale -lavutil -lm \
	-pthread \
	-lworkerfs.js \
	-o www/av.js \
	src/demo3.cpp \

