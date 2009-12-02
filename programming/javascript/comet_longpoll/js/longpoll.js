var Comet = {
	longpoll: function (lm) {
		var xhReq = new XMLHttpRequest ();

		xhReq.open ("GET", "/broadcast/sub?id=3", true);
		if (lm) {
			xhReq.setRequestHeader ('If-Modified-Since', lm);
		} else {
			xhReq.setRequestHeader("If-Modified-Since", new Date (0).toGMTString ());
		}
		xhReq.onreadystatechange = function () {
			if (xhReq.readyState != 4) { return; }
			document.getElementById ('message').innerHTML = xhReq.responseText;
			if (lm == xhReq.getResponseHeader ('Last-Modified')) {
				Comet.longpoll (new Date (Date.parse (lm) + 1000).toGMTString ());
			} else {
				Comet.longpoll (xhReq.getResponseHeader ('Last-Modified'));
			}
		}
		xhReq.send (null);
	}
}
