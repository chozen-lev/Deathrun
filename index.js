var server 			= require("./server"),
	router 			= require("./router"),
	requestHandlers = require("./requestHandlers");

var handle = {};
handle["/start"]  = requestHandlers.start;
handle["/"] 	  = requestHandlers.start;
handle["/upload"] = requestHandlers.upload;
handle["/show"]	  = requestHandlers.show;

server.start(router.route, handle);