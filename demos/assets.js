
  var Module = typeof Module != 'undefined' ? Module : {};

  if (!Module['expectedDataFileDownloads']) Module['expectedDataFileDownloads'] = 0;
  Module['expectedDataFileDownloads']++;
  (() => {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    var isPthread = typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD;
    var isWasmWorker = typeof ENVIRONMENT_IS_WASM_WORKER != 'undefined' && ENVIRONMENT_IS_WASM_WORKER;
    if (isPthread || isWasmWorker) return;
    var isNode = globalThis.process && globalThis.process.versions && globalThis.process.versions.node && globalThis.process.type != 'renderer';
    async function loadPackage(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.substring(0, window.location.pathname.lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.substring(0, location.pathname.lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = '/home/runner/work/gpr300-sokol/gpr300-sokol/build/web/demos/Release//assets.data';
      var REMOTE_PACKAGE_BASE = 'assets.data';
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
      var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

      async function fetchRemotePackage(packageName, packageSize) {
        if (isNode) {
          var contents = require('fs').readFileSync(packageName);
          return new Uint8Array(contents).buffer;
        }
        if (!Module['dataFileDownloads']) Module['dataFileDownloads'] = {};
        try {
          var response = await fetch(packageName);
        } catch (e) {
          throw new Error(`Network Error: ${packageName}`, {e});
        }
        if (!response.ok) {
          throw new Error(`${response.status}: ${response.url}`);
        }

        const chunks = [];
        const headers = response.headers;
        const total = Number(headers.get('Content-Length') || packageSize);
        let loaded = 0;

        Module['setStatus'] && Module['setStatus']('Downloading data...');
        const reader = response.body.getReader();

        while (1) {
          var {done, value} = await reader.read();
          if (done) break;
          chunks.push(value);
          loaded += value.length;
          Module['dataFileDownloads'][packageName] = {loaded, total};

          let totalLoaded = 0;
          let totalSize = 0;

          for (const download of Object.values(Module['dataFileDownloads'])) {
            totalLoaded += download.loaded;
            totalSize += download.total;
          }

          Module['setStatus'] && Module['setStatus'](`Downloading data... (${totalLoaded}/${totalSize})`);
        }

        const packageData = new Uint8Array(chunks.map((c) => c.length).reduce((a, b) => a + b, 0));
        let offset = 0;
        for (const chunk of chunks) {
          packageData.set(chunk, offset);
          offset += chunk.length;
        }
        return packageData.buffer;
      }

      var fetchPromise;
      var fetched = Module['getPreloadedPackage']?.(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE);

      if (!fetched) {
        // Note that we don't use await here because we want to execute the
        // the rest of this function immediately.
        fetchPromise = fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE);
      }

    async function runWithFS(Module) {

      function assert(check, msg) {
        if (!check) throw new Error(msg);
      }
Module['FS_createPath']("/", "assets", true, true);
Module['FS_createPath']("/assets", "doubledash", true, true);
Module['FS_createPath']("/assets", "environment", true, true);
Module['FS_createPath']("/assets", "heightmaps", true, true);
Module['FS_createPath']("/assets", "shaders", true, true);
Module['FS_createPath']("/assets/shaders", "deferred", true, true);
Module['FS_createPath']("/assets/shaders", "postprocess", true, true);
Module['FS_createPath']("/assets/shaders", "sunshine", true, true);
Module['FS_createPath']("/assets/shaders", "windwaker", true, true);
Module['FS_createPath']("/assets", "skybox", true, true);
Module['FS_createPath']("/assets", "smashbros", true, true);
Module['FS_createPath']("/assets/smashbros", "greenshell", true, true);
Module['FS_createPath']("/assets/smashbros", "togezoshell", true, true);
Module['FS_createPath']("/assets", "sunshine", true, true);
Module['FS_createPath']("/assets", "transitions", true, true);
Module['FS_createPath']("/assets", "windwaker", true, true);
Module['FS_createPath']("/assets/windwaker", "big_crate", true, true);
Module['FS_createPath']("/assets/windwaker", "island", true, true);
Module['FS_createPath']("/assets/windwaker", "pot_water", true, true);
Module['FS_createPath']("/assets/windwaker", "skull", true, true);

      for (var file of metadata['files']) {
        var name = file['filename']
        Module['addRunDependency'](`fp ${name}`);
      }

      async function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData ' + arrayBuffer.constructor.name);
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          for (var file of metadata['files']) {
            var name = file['filename'];
            var data = byteArray.subarray(file['start'], file['end']);
            // canOwn this data in the filesystem, it is a slice into the heap that will never change
          Module['FS_createDataFile'](name, null, data, true, true, true);
          Module['removeRunDependency'](`fp ${name}`);
          }
          Module['removeRunDependency']('datafile_/home/runner/work/gpr300-sokol/gpr300-sokol/build/web/demos/Release//assets.data');
      }
      Module['addRunDependency']('datafile_/home/runner/work/gpr300-sokol/gpr300-sokol/build/web/demos/Release//assets.data');

      if (!Module['preloadResults']) Module['preloadResults'] = {};

      Module['preloadResults'][PACKAGE_NAME] = {fromCache: false};
      if (!fetched) {
        fetched = await fetchPromise;
      }
      processPackageData(fetched);

    }
    if (Module['calledRun']) {
      runWithFS(Module);
    } else {
      if (!Module['preRun']) Module['preRun'] = [];
      Module['preRun'].push(runWithFS); // FS is not initialized yet, wait for it
    }

    }
    loadPackage({"files": [{"filename": "/assets/brick_color.jpg", "start": 0, "end": 1550678}, {"filename": "/assets/doubledash/water.fs", "start": 1550678, "end": 1552187}, {"filename": "/assets/doubledash/water.vs", "start": 1552187, "end": 1552687}, {"filename": "/assets/doubledash/wave_spec.png", "start": 1552687, "end": 1564833}, {"filename": "/assets/doubledash/wave_tex.png", "start": 1564833, "end": 1578368}, {"filename": "/assets/doubledash/wave_warp.png", "start": 1578368, "end": 1584401}, {"filename": "/assets/environment/grass.png", "start": 1584401, "end": 2452259}, {"filename": "/assets/environment/ice.png", "start": 2452259, "end": 5107467}, {"filename": "/assets/environment/rock.png", "start": 5107467, "end": 6299239}, {"filename": "/assets/environment/sand.png", "start": 6299239, "end": 8265687}, {"filename": "/assets/environment/stone.png", "start": 8265687, "end": 10173966}, {"filename": "/assets/heightmaps/heightmap.png", "start": 10173966, "end": 10315391}, {"filename": "/assets/heightmaps/heightmap2.png", "start": 10315391, "end": 10492639}, {"filename": "/assets/heightmaps/heightmap3.png", "start": 10492639, "end": 10543085}, {"filename": "/assets/shaders/blinnphong.fs", "start": 10543085, "end": 10544478}, {"filename": "/assets/shaders/blinnphong.vs", "start": 10544478, "end": 10544960}, {"filename": "/assets/shaders/deferred/blinnphong.fs", "start": 10544960, "end": 10546457}, {"filename": "/assets/shaders/deferred/blinnphong.vs", "start": 10546457, "end": 10546789}, {"filename": "/assets/shaders/deferred/default.fs", "start": 10546789, "end": 10547101}, {"filename": "/assets/shaders/deferred/default.vs", "start": 10547101, "end": 10547332}, {"filename": "/assets/shaders/deferred/geometry.fs", "start": 10547332, "end": 10548064}, {"filename": "/assets/shaders/deferred/geometry.vs", "start": 10548064, "end": 10548643}, {"filename": "/assets/shaders/deferred/light.fs", "start": 10548643, "end": 10548783}, {"filename": "/assets/shaders/deferred/light.vs", "start": 10548783, "end": 10549115}, {"filename": "/assets/shaders/deferred/pbr.fs", "start": 10549115, "end": 10553076}, {"filename": "/assets/shaders/deferred/pbr.vs", "start": 10553076, "end": 10553408}, {"filename": "/assets/shaders/depth.fs", "start": 10553408, "end": 10553438}, {"filename": "/assets/shaders/depth.vs", "start": 10553438, "end": 10553696}, {"filename": "/assets/shaders/island.fs", "start": 10553696, "end": 10555586}, {"filename": "/assets/shaders/island.vs", "start": 10555586, "end": 10556884}, {"filename": "/assets/shaders/island_water.fs", "start": 10556884, "end": 10557395}, {"filename": "/assets/shaders/island_water.vs", "start": 10557395, "end": 10557949}, {"filename": "/assets/shaders/pbr.fs", "start": 10557949, "end": 10562392}, {"filename": "/assets/shaders/pbr.vs", "start": 10562392, "end": 10562900}, {"filename": "/assets/shaders/postprocess/blur.fs", "start": 10562900, "end": 10563800}, {"filename": "/assets/shaders/postprocess/chromaticaberration.fs", "start": 10563800, "end": 10564251}, {"filename": "/assets/shaders/postprocess/crt.fs", "start": 10564251, "end": 10565798}, {"filename": "/assets/shaders/postprocess/default.fs", "start": 10565798, "end": 10566007}, {"filename": "/assets/shaders/postprocess/default.vs", "start": 10566007, "end": 10566238}, {"filename": "/assets/shaders/postprocess/edgedetection.fs", "start": 10566238, "end": 10567097}, {"filename": "/assets/shaders/postprocess/grayscale.fs", "start": 10567097, "end": 10567400}, {"filename": "/assets/shaders/postprocess/inverse.fs", "start": 10567400, "end": 10567615}, {"filename": "/assets/shaders/postprocess/pixelation.fs", "start": 10567615, "end": 10567975}, {"filename": "/assets/shaders/shadow.fs", "start": 10567975, "end": 10570559}, {"filename": "/assets/shaders/shadow.vs", "start": 10570559, "end": 10571135}, {"filename": "/assets/shaders/snow.fs", "start": 10571135, "end": 10572635}, {"filename": "/assets/shaders/snow.vs", "start": 10572635, "end": 10574293}, {"filename": "/assets/shaders/spline.fs", "start": 10574293, "end": 10574429}, {"filename": "/assets/shaders/spline.vs", "start": 10574429, "end": 10574652}, {"filename": "/assets/shaders/sunshine/water.fs", "start": 10574652, "end": 10575920}, {"filename": "/assets/shaders/sunshine/water.vs", "start": 10575920, "end": 10576282}, {"filename": "/assets/shaders/transition.fs", "start": 10576282, "end": 10576654}, {"filename": "/assets/shaders/transition.vs", "start": 10576654, "end": 10576887}, {"filename": "/assets/shaders/windwaker/toonshading.fs", "start": 10576887, "end": 10577947}, {"filename": "/assets/shaders/windwaker/toonshading.vs", "start": 10577947, "end": 10578431}, {"filename": "/assets/shaders/windwaker/water.fs", "start": 10578431, "end": 10580005}, {"filename": "/assets/shaders/windwaker/water.vs", "start": 10580005, "end": 10580941}, {"filename": "/assets/skybox/back.jpg", "start": 10580941, "end": 11321009}, {"filename": "/assets/skybox/bottom.jpg", "start": 11321009, "end": 11601598}, {"filename": "/assets/skybox/front.jpg", "start": 11601598, "end": 12074927}, {"filename": "/assets/skybox/left.jpg", "start": 12074927, "end": 12676812}, {"filename": "/assets/skybox/right.jpg", "start": 12676812, "end": 13214850}, {"filename": "/assets/skybox/top.jpg", "start": 13214850, "end": 13560989}, {"filename": "/assets/smashbros/greenshell/greenshell.mtl", "start": 13560989, "end": 13561419}, {"filename": "/assets/smashbros/greenshell/greenshell.obj", "start": 13561419, "end": 14275382}, {"filename": "/assets/smashbros/greenshell/greenshell_ao.png", "start": 14275382, "end": 14283783}, {"filename": "/assets/smashbros/greenshell/greenshell_col.png", "start": 14283783, "end": 14324582}, {"filename": "/assets/smashbros/greenshell/greenshell_mtl.png", "start": 14324582, "end": 14327972}, {"filename": "/assets/smashbros/greenshell/greenshell_pbr.png", "start": 14327972, "end": 14335820}, {"filename": "/assets/smashbros/greenshell/greenshell_rgh.png", "start": 14335820, "end": 14339358}, {"filename": "/assets/smashbros/greenshell/greenshell_spc.png", "start": 14339358, "end": 14341541}, {"filename": "/assets/smashbros/togezoshell/togezoshell.mtl", "start": 14341541, "end": 14341982}, {"filename": "/assets/smashbros/togezoshell/togezoshell.obj", "start": 14341982, "end": 15432474}, {"filename": "/assets/smashbros/togezoshell/togezoshell_ao.png", "start": 15432474, "end": 15458250}, {"filename": "/assets/smashbros/togezoshell/togezoshell_col.png", "start": 15458250, "end": 15510414}, {"filename": "/assets/smashbros/togezoshell/togezoshell_mtl.png", "start": 15510414, "end": 15519810}, {"filename": "/assets/smashbros/togezoshell/togezoshell_rgh.png", "start": 15519810, "end": 15537107}, {"filename": "/assets/smashbros/togezoshell/togezoshell_spc.png", "start": 15537107, "end": 15541480}, {"filename": "/assets/snow.png", "start": 15541480, "end": 15577369}, {"filename": "/assets/sponza.obj", "start": 15577369, "end": 39432607}, {"filename": "/assets/sunshine/water.fs", "start": 39432607, "end": 39433883}, {"filename": "/assets/sunshine/water.vs", "start": 39433883, "end": 39434251}, {"filename": "/assets/sunshine/water128.png", "start": 39434251, "end": 39434654}, {"filename": "/assets/sunshine/water16.png", "start": 39434654, "end": 39435118}, {"filename": "/assets/sunshine/water32.png", "start": 39435118, "end": 39436530}, {"filename": "/assets/sunshine/water64.png", "start": 39436530, "end": 39440629}, {"filename": "/assets/sunshine/water8.png", "start": 39440629, "end": 39440720}, {"filename": "/assets/suzanne.obj", "start": 39440720, "end": 39515601}, {"filename": "/assets/transitions/gradient1.png", "start": 39515601, "end": 39519003}, {"filename": "/assets/transitions/gradient2.png", "start": 39519003, "end": 39530990}, {"filename": "/assets/transitions/gradient3.png", "start": 39530990, "end": 39538473}, {"filename": "/assets/windwaker/Txe_Ecube_yoko_9.png", "start": 39538473, "end": 39560921}, {"filename": "/assets/windwaker/ZAtoon 2.png", "start": 39560921, "end": 39563657}, {"filename": "/assets/windwaker/ZAtoon.png", "start": 39563657, "end": 39566394}, {"filename": "/assets/windwaker/big_crate/Txo_spbox.png", "start": 39566394, "end": 39568070}, {"filename": "/assets/windwaker/big_crate/big_crate.mtl", "start": 39568070, "end": 39568321}, {"filename": "/assets/windwaker/big_crate/big_crate.obj", "start": 39568321, "end": 39583892}, {"filename": "/assets/windwaker/island/Island.mtl", "start": 39583892, "end": 39586701}, {"filename": "/assets/windwaker/island/Island.obj", "start": 39586701, "end": 40089777}, {"filename": "/assets/windwaker/island/OutsMM02.png", "start": 40089777, "end": 40174016}, {"filename": "/assets/windwaker/island/OutsMM03.png", "start": 40174016, "end": 40365743}, {"filename": "/assets/windwaker/island/OutsSS00.png", "start": 40365743, "end": 40395583}, {"filename": "/assets/windwaker/island/OutsSS01.png", "start": 40395583, "end": 40436247}, {"filename": "/assets/windwaker/island/OutsSS04.png", "start": 40436247, "end": 40476714}, {"filename": "/assets/windwaker/island/OutsSS05.png", "start": 40476714, "end": 40530449}, {"filename": "/assets/windwaker/island/OutsSS06.png", "start": 40530449, "end": 40541161}, {"filename": "/assets/windwaker/island/OutsSS07.png", "start": 40541161, "end": 40583646}, {"filename": "/assets/windwaker/island/Sea.SS00.png", "start": 40583646, "end": 40587149}, {"filename": "/assets/windwaker/island/Sea.SS01.png", "start": 40587149, "end": 40589116}, {"filename": "/assets/windwaker/island/Sea.SS02.png", "start": 40589116, "end": 40590851}, {"filename": "/assets/windwaker/island/Sea.SS04.png", "start": 40590851, "end": 40592101}, {"filename": "/assets/windwaker/island/Sea.SS05.png", "start": 40592101, "end": 40593098}, {"filename": "/assets/windwaker/island/Sea.SS06.png", "start": 40593098, "end": 40594414}, {"filename": "/assets/windwaker/island/Sea.SS07.png", "start": 40594414, "end": 40596492}, {"filename": "/assets/windwaker/island/Sea.SS08.png", "start": 40596492, "end": 40598180}, {"filename": "/assets/windwaker/island/Sea.mtl", "start": 40598180, "end": 40600852}, {"filename": "/assets/windwaker/island/Sea.obj", "start": 40600852, "end": 40674925}, {"filename": "/assets/windwaker/island/WindSS00.png", "start": 40674925, "end": 40675917}, {"filename": "/assets/windwaker/island/window_lights.mtl", "start": 40675917, "end": 40676353}, {"filename": "/assets/windwaker/island/window_lights.obj", "start": 40676353, "end": 40678503}, {"filename": "/assets/windwaker/pot_water/k_mtuybo00.png", "start": 40678503, "end": 40679635}, {"filename": "/assets/windwaker/pot_water/pot_water.mtl", "start": 40679635, "end": 40679889}, {"filename": "/assets/windwaker/pot_water/pot_water.obj", "start": 40679889, "end": 40692791}, {"filename": "/assets/windwaker/skull/Txo_dokuo.png", "start": 40692791, "end": 40694793}, {"filename": "/assets/windwaker/skull/skull.mtl", "start": 40694793, "end": 40695042}, {"filename": "/assets/windwaker/skull/skull.obj", "start": 40695042, "end": 40708686}, {"filename": "/assets/windwaker/water.png", "start": 40708686, "end": 40864225}, {"filename": "/assets/windwaker/water128.png", "start": 40864225, "end": 40868822}, {"filename": "/assets/windwaker/water16.png", "start": 40868822, "end": 40868910}, {"filename": "/assets/windwaker/water32.png", "start": 40868910, "end": 40869687}, {"filename": "/assets/windwaker/water64.png", "start": 40869687, "end": 40871948}, {"filename": "/assets/windwaker/water8.png", "start": 40871948, "end": 40872033}], "remote_package_size": 40872033});

  })();
