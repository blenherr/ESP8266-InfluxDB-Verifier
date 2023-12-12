/*
 * ESP8266 InfluxDB Verifier
 * AUTHOR: Benjamin Lenherr
 * https://github.com/blenherr/ESP8266-InfluxDB-Verifier
 *
 * This is a way to verify your InfluxDB configuration
 * on a ESP8266 while AsyncWebServer is running.
 */

function _id(el) {
	return document.getElementById(el);
}

function onLoadConfig() {
	influxVer();
}

function influxVer() {
	if (_id('influx_ver_1').checked) {
		_id('influx_db_tr').style.display = 'table-row';
		_id('influx_db').disabled = false;
		_id('influx_auth_tr').style.display = 'table-row';
		_id('influx_auth_0').disabled = false;
		_id('influx_auth_1').disabled = false;
		_id('influx_buck_tr').style.display = 'none';
		_id('influx_buck').disabled = true;
		_id('influx_org_tr').style.display = 'none';
		_id('influx_org').disabled = true;
		_id('influx_tok_tr').style.display = 'none';
		_id('influx_tok').disabled = true;
		influxAuth();
	}
	if (_id('influx_ver_2').checked) {
		_id('influx_db_tr').style.display = 'none';
		_id('influx_db').disabled = true;
		_id('influx_auth_tr').style.display = 'none';
		_id('influx_auth_0').disabled = true;
		_id('influx_auth_1').disabled = true;
		_id('influx_buck_tr').style.display = 'table-row';
		_id('influx_buck').disabled = false;
		_id('influx_org_tr').style.display = 'table-row';
		_id('influx_org').disabled = false;
		_id('influx_tok_tr').style.display = 'table-row';
		_id('influx_tok').disabled = false;
		influxAuth();
	}
}

function influxAuth() {
	if (_id('influx_auth_0').checked && !_id('influx_auth_0').disabled) {
		_id('influx_user_tr').style.display = 'none';
		_id('influx_user').disabled = true;
		_id('influx_pass_tr').style.display = 'none';
		_id('influx_pass').disabled = true;
	}
	else if (_id('influx_auth_1').checked && !_id('influx_auth_1').disabled) {
		_id('influx_user_tr').style.display = 'table-row';
		_id('influx_user').disabled = false;
		_id('influx_pass_tr').style.display = 'table-row';
		_id('influx_pass').disabled = false;
	}
	else {
		_id('influx_user_tr').style.display = 'none';
		_id('influx_user').disabled = true;
		_id('influx_pass_tr').style.display = 'none';
		_id('influx_pass').disabled = true;
	}
}

function influxTest() {
	_id('influx_testing').innerHTML = 'Testing...';
	_id('influx_result_tr').style.display = 'none';
	_id('influx_version').innerHTML = '';
	_id('influx_result').innerHTML = '';
	if (!RegExp('https?://+[A-Za-z0-9_\\-.]+:([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$').test(_id('influx_url').value)) {
		_id('influx_testing').innerHTML = 'Form input incorrect';
		_id('influx_result_tr').style.display = 'table-row';
		_id('influx_result').innerHTML = 'Url not valid';
		return;
	}
	if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_meas').value)) {
		_id('influx_testing').innerHTML = 'Form input incorrect';
		_id('influx_result_tr').style.display = 'table-row';
		_id('influx_result').innerHTML = 'Measurement not valid';
		return;
	}
	if (_id('influx_ver_1').checked) {
		if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_db').value)) {
			_id('influx_testing').innerHTML = 'Form input incorrect';
			_id('influx_result_tr').style.display = 'table-row';
			_id('influx_result').innerHTML = 'Database not valid';
			return;
		}
		if (_id('influx_auth_1').checked) {
			if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_user').value)) {
				_id('influx_testing').innerHTML = 'Form input incorrect';
				_id('influx_result_tr').style.display = 'table-row';
				_id('influx_result').innerHTML = 'Username not valid';
				return;
			}
			if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_pass').value)) {
				_id('influx_testing').innerHTML = 'Form input incorrect';
				_id('influx_result_tr').style.display = 'table-row';
				_id('influx_result').innerHTML = 'Password not valid';
				return;
			}
		}
	}
	if (_id('influx_ver_2').checked) {
		if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_buck').value)) {
			_id('influx_testing').innerHTML = 'Form input incorrect';
			_id('influx_result_tr').style.display = 'table-row';
			_id('influx_result').innerHTML = 'Bucket not valid';
			return;
		}
		if (!RegExp('^[A-Za-z0-9]{3,32}$').test(_id('influx_org').value)) {
			_id('influx_testing').innerHTML = 'Form input incorrect';
			_id('influx_result_tr').style.display = 'table-row';
			_id('influx_result').innerHTML = 'Organisation not valid';
			return;
		}
		if (!RegExp('^[A-Za-z0-9\-_]{18,98}==$').test(_id('influx_tok').value)) {
			_id('influx_testing').innerHTML = 'Form input incorrect';
			_id('influx_result_tr').style.display = 'table-row';
			_id('influx_result').innerHTML = 'Tocken not valid';
			return;
		}
	}
	const XHR = new XMLHttpRequest();
	const FD = new FormData();
	FD.append('influx_url', _id('influx_url').value);
	FD.append('influx_meas', _id('influx_meas').value);
	if (_id('influx_ver_1').checked) {
		FD.append('influx_ver', '1');
		FD.append('influx_db', _id('influx_db').value);
		if (_id('influx_auth_1').checked) {
			FD.append('influx_auth', '1');
			FD.append('influx_user', _id('influx_user').value);
			FD.append('influx_pass', _id('influx_pass').value);
		}
		else
			FD.append('influx_auth', '0');
	}
	if (_id('influx_ver_2').checked) {
		FD.append('influx_ver', '2');
		FD.append('influx_buck', _id('influx_buck').value);
		FD.append('influx_org', _id('influx_org').value);
		FD.append('influx_tok', _id('influx_tok').value);
	}
	XHR.open('POST', '/influx_test', true);
	XHR.setRequestHeader('Accept', 'text/plain');
	XHR.addEventListener('load', function () {
		if (this.readyState == 4 && this.status == 200) {
			const interval = setInterval(function () {
				const XHR = new XMLHttpRequest();
				XHR.open('GET', '/influx_progress', true);
				XHR.setRequestHeader('Content-Type', 'text/plain; charset=utf-8');
				XHR.setRequestHeader('Accept', 'text/plain');
				XHR.onreadystatechange = function () {
					if (this.readyState == 4 && this.status == 200) {
						if (this.responseText.length) {
							_id('influx_testing').innerHTML = 'Completed';
							_id('influx_result_tr').style.display = 'table-row';
							const obj = JSON.parse(this.responseText);
							if ('version' in obj)
								_id('influx_version').innerHTML = 'Version: ' + obj.version + '<br>';
							else
								_id('influx_version').innerHTML = 'Version: no result object<br>';
							if ('result' in obj)
								_id('influx_result').innerHTML = obj.result;
							else
								_id('influx_result').innerHTML = 'No result object';
							clearInterval(interval);
							return;
						}
					}
					else if (this.readyState == 4 && this.status != 200) {
						_id('influx_testing').innerHTML = 'Request failed';
						_id('influx_result_tr').style.display = 'table-row';
						_id('influx_result').innerHTML = 'This Wemos\'s server not responding';
						clearInterval(interval);
						return;
					}
				};
				XHR.send();
			}, 1000);
		}
		else if (this.readyState == 4 && this.status != 200) {
			_id('influx_testing').innerHTML = 'Request failed';
			_id('influx_result_tr').style.display = 'table-row';
			_id('influx_result').innerHTML = 'This Wemos\'s server not responding';
			return;
		}
	});
	XHR.addEventListener('error', function () {
		_id('influx_testing').innerHTML = 'Request failed';
		_id('influx_result_tr').style.display = 'table-row';
		_id('influx_result').innerHTML = 'This Wemos\'s server not responding';
		return;
	});
	XHR.send(FD);
}