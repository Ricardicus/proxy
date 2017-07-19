function update_settings() {

	http_get("proxy.cgi?action=get", "json", function(reponse){

		var port = reponse["Port"];
		var host = reponse["Host"];
		var proxy_port = reponse["Proxy_Port"];

		if ( typeof port == 'undefined') {
			console.log("Error: proxy host undefined");
			return;
		} else if ( typeof host == 'undefined') {
			console.log("Error: proxy port undefined");
			return;
		} else if ( typeof proxy_port == 'undefined') {
			console.log("Error: proxy_port undefined");
			return;
		}

		var href =  window.location.protocol + "//" + window.location.hostname + ":" + proxy_port + "/";

		$("#current_proxy_host").html(host);
		$("#current_proxy_port").html(port);
		$("#current_proxy_link").html("Go to proxy").attr("href", href);

	});

}

function init_settings() {
	
	// Graphic stuff
	$(".overout")
  	.mouseover(function() {
	  	$(this).fadeTo(100,0.5);
	})
	.mouseout(function() {
	    $(this).fadeTo(100,1.0);
	})
	.unbind("click")
	.click(function(){

		var host = $("#set_current_proxy_host").val();
		var port = $("#set_current_proxy_port").val();

		http_get("proxy.cgi?action=set&host=" + host + "&port=" + port, "text", function(reponse){
			update_settings();
		});

	})

	;


}