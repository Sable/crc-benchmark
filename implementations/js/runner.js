function runner (pageSize, numPages, numExecs) {
  var data = randCRC(numPages, pageSize)
  var numWords = pageSize / 4

  if ((pageSize % 4) !== 0) {
    server.error("Unsupported page size of '" + pageSize +
      "' please choose a page size that is a multiple of 4")
  }

  var crcs = new Uint32Array(numPages) // TA
  var cumulativeTime = 0

  var expectedCrc = 2231263667
  var t1 = performance.now()
  for (var j = 0; j < numExecs; ++j) {
    for (var i = 0; i < numPages; ++i) {
      crcs[i] = crc32_8bytes(data, i * numWords, pageSize)
    }

    // crc32_8bytes is expecting the length in bytes
    var finalCrc = crc32_8bytes(crcs, 0, numPages * 4)
    // Convert expectedCrc to signed 32 bit using implicit integer
    // coercion because JS does not have an unsigned 32 bit data type
    // to compare against
    if (pageSize === 65536 &&
      numPages === 128 &&
      numExecs === 150) {
      if (finalCrc !== (expectedCrc | 0)) {
        server.error("Invalid crc check, received '" + finalCrc + "' while expecting '" + (expectedCrc | 0) + "'(signed) [or '" + (expectedCrc) + "'(unsigned)]")
      }
    } else {
      server.log('Warning: No self-checking step for (' + pageSize + ',' + numPages + ',' + numExecs + ')')
    }
  }
  cumulativeTime += performance.now() - t1

  console.log('Total time was ' + cumulativeTime / 1000 + ' s')
  console.log(JSON.stringify({ status: 1,
    options: 'run(' + [numPages, pageSize, numExecs].join(',') + ')',
  time: cumulativeTime / 1000 }))
}
