function http_get(url, dataType, successCallback, errorCallback){

	$.ajax({
		type: "GET",
		url: url,
		dataType: dataType,
		success: function(response){
			if ( !(typeof successCallback === 'undefined') ) {
				successCallback(response);
			}
		},
		error: function(){
			if ( !(typeof errorCallback === 'undefined') ) {
				errorCallback();
			}
		}
	});

}