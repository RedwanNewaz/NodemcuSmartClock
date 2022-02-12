var http = require('http'),
    fileSystem = require('fs'),
    path = require('path'),
    util = require('util');

console.log('Azan Server running on 8081 ....')

http.createServer(function(request, response) {

    if(request.url == '/mp3/azan1.mp3')
    {
        var filePath = `${__dirname}/mp3/azan1.mp3`;
        console.log(filePath);
        var stat = fileSystem.statSync(filePath);
    
        response.writeHead(200, {
            'Content-Type': 'audio/mpeg',
            'Content-Length': stat.size
        });    
        fileSystem.createReadStream(filePath).pipe(response);
    }

})
.listen(8081);