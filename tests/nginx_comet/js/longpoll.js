function handleResponse ()
{
	if (this.readyState != 4) {
		return;
	}
	//if (!this.responseText) { return; }

	//document.getElementById ('message').innerHTML = this.responseText;
	document.getElementById ('message').innerHTML = this.responseText + " " + this.getResponseHeader ('Last-Modified');
	//longpoll (this);
}

function xh_onload ()
{
	if (this.responseText) {
		document.getElementById ('message').innerHTML = this.responseText;
	}
}

function longpoll (xhr)
{
	var xhReq = new XMLHttpRequest ();
/*
	if (!xhr) {
		xhr = new XMLHttpRequest ();
//		xhr.setRequestHeader ("If-Modified-Since", "Sat, 28 Nov 2009 00:00:00 GMT");
	} else {
		xhr.setRequestHeader ("If-Modified-Since", xhr.getResponseHeader ('Last-Modified'));
	}
*/

	if (xhr) {
		xhr.setRequestHeader ("If-Modified-Since", xhr.getResponseHeader ('Last-Modified'));
	}
	xhReq.onreadystatechange = handleResponse;
	xhReq.open ("GET", "/broadcast/sub", true);
	xhReq.send (null);
}
