var server = "http://127.0.0.1:5000";
var clients = [];
var actions = [];
var width = 1;
var height = 1;
var field = [];
var canvas = document.getElementById("draw_canvas");
var context = canvas.getContext('2d');

function query(width, height, data) {
	var url = `${server}/api/calc?w=${width}&h=${height}&data=${data}&key=actions,clients`;
	console.log(url);
	fetch(url, {
		credentials: 'include',
		method: 'GET',
	})
	.then((response) => response.json())
	.then((json) => {
		clients = json['data']['clients'];
		actions = json['data']['actions'];
		animation();
	});

}
var clients_count = 0;
var formfield = document.getElementById('form_clients');


function add_clicked() {

	var newField = document.createElement('div');
	newField.setAttribute('id','client' + clients_count);

	formfield.appendChild(newField);
	clients_count++;
	document.getElementById(`client${clients_count - 1}`).setHTML(`<div> \
		<label>Client ${clients_count}</label><br>\
		<input value=1 style="width: 5em" type="number" id="client-in-${clients_count - 1}" />\
		<input value=2 style="width: 5em" type="number" id="client-out-${clients_count - 1}" />\
		</div>`);


}
function del_clicked() {

	if (clients_count > 1) {
		document.getElementById(`client${clients_count - 1}`).remove();
		clients_count--;
	}
	
	
}

function draw() {

	context.clearRect(0, 0, canvas.width, canvas.height);
	var cell_size = Math.floor(710 / Math.max(width, height));
	for (var i = 0; i < height; i++) {
		for (var j = 0; j < width; j++) {
			context.fillStyle = "#eee";
			context.strokeStyle = "#000";
			context.lineWidth = 2;

			if (field[i][j] != -1) {
				context.fillStyle = "#eef";
				context.strokeStyle = "#00a";
				context.lineWidth = 4;


			}
			context.fillRect(5 + j * cell_size, 5 + i * cell_size, cell_size, cell_size);
			context.strokeRect(5 + j * cell_size, 5 + i * cell_size, cell_size, cell_size);
		}
	}

}

function move_client(id, row, col, value) {
	console.log(id, row, col, value);
	field[row][col] = value;
	draw();
}

function animation () {


	field = [];
	for (var i = 0; i < height; i++) {
		field.push([]);
		for (var j = 0; j < width; j++) {
			field[field.length - 1].push(-1);
		}
	}
	draw();
	var delay = 150;
	for (var i = 0; i < actions.length; i++) {

		var type = -1;

		if (actions[i]['type'] == 0) {
			type = 0;
		}
		setTimeout(move_client, delay * (i + 1), actions[i]['id'], actions[i]['row'], actions[i]['column'], type);
	}

}

function solve_clicked() {
	width = document.getElementById('parking-width').value;
	height = document.getElementById('parking-height').value;
	if (height < 1 || width < 1 || height > 100 || width > 100) {
		return;
	}
	var data = "";
	for (var i = 0; i < clients_count; i++) {
		if (i > 0) data += ";";
		var first = document.getElementById(`client-in-${i}`).value;
		var second = document.getElementById(`client-out-${i}`).value;
		if (first < 0 || second < 0 || first > 1000000000 || second > 1000000000) {
			return;
		}
		data += first + ',' + second;
	}

	query(width, height, data);
	
}

add_clicked();