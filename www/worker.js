const vector2Array = (vector) => {
    let r = []
    for (let i = 0; i < vector.size(); i++) {
        const e = vector.get(i);
        r.push(e)
        // console.log('response:', res);
    }
    return r
}
onmessage = (msg) => {
    const file = msg.data;

    // Create and mount FS work directory.
    if (!FS.analyzePath('/work').exists) {
        FS.mkdir('/work');
    }
    FS.mount(WORKERFS, { files: [file] }, '/work');

    // Run the Wasm function we exported.
    const info = Module.run('/work/' + file.name);

    // Post message back to main thread.
    postMessage(vector2Array(info));
    // console.log('post info', info);
    // Unmount the work directory.
    FS.unmount('/work');
}

// Import the Wasm loader generated from our Emscripten build.
self.importScripts('av.js');