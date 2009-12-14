var Comet = {
	longpoll: function (lm) {
		var xhReq = new XMLHttpRequest ();

		xhReq.open ("GET", "/broadcast/sub?id=3", true);
		if (lm) {
			xhReq.setRequestHeader ('If-Modified-Since', lm);
		} else {
			xhReq.setRequestHeader ('Cache-Control', 'no-cache');
			xhReq.setRequestHeader ('Pragma', 'no-cache');
		}
		xhReq.onreadystatechange = function () {
			if (xhReq.readyState != 4) { return; }
			document.getElementById ('message').innerHTML = xhReq.responseText;
			Comet.longpoll (xhReq.getResponseHeader ('Last-Modified'));
		}
		xhReq.send (null);
	}
}