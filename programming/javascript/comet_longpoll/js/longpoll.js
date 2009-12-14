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
			if (lm == xhReq.getResponseHeader ('Last-Modified') || !xhReq.responseText) {
				Comet.longpoll (new Date (Date.parse (lm) + 1000).toGMTString ());
			} else {
				Comet.longpoll (xhReq.getResponseHeader ('Last-Modified'));
			}
		}
		xhReq.send (null);
	}
}

function handleSubmit (form) {
	var xhr, value;

	value = form.elements['value'].value;
	if (value == "") {
		document.getElementById ('message').innerHTML = "Valor nao pode ser nulo";
		return false;
	}

	xhr = new XMLHttpRequest ();

	xhr.open ("POST", "/broadcast/pub?id=3", false);
	xhr.send (value);

	document.getElementById ('message').innerHTML = "Valor enviado: " + value;

	return false;
}
