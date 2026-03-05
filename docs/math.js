

// LANGAGE

let translations = {};
let currentLang = "en";

async function loadLanguage(lang)
{
	const response = await fetch(`locales/${lang}.json`);
	translations = await response.json();
	currentLang = lang;
	updateTexts();
}

// function t(key)
// {
// 	return translations[key] || key;
// }
function t(path)
{
	return path.split(".")
		.reduce((obj, key) => obj?.[key], translations) || path;
}
//	Equivalent:
// 	function t(path) {
// 	const keys = path.split(".");
// 	let obj = translations;

// 	for (let key of keys) {
// 		if (obj == null) return path;
// 		obj = obj[key];
// 	}

// 	return obj || path;
// }

function updateTexts()
{
	document.querySelectorAll("[data-i18n]")
		.forEach(el => 
		{
		const key = el.getAttribute("data-i18n");
		el.textContent = t(key);
		});
}
loadLanguage(currentLang);
document.getElementById("langSelect").addEventListener
	("change",
	(e) => {loadLanguage(e.target.value);}
	);

// C API

const errMessages =
{
	en:
	{
		1 : "unexpected closing parenthesis",
		2 : "expected closing parenthesis",
		3 : "operation with no operands",
		4 : "missing operation",
		5 : "undefined token",
		6 : "invalid space"
	},
}
let language = "en";
var Module =
{
	print(text)
	{
		// document.getElementById("output").textContent += text + "\n";
		const mathSpan = document.createElement("span");
		katex.render(text, mathSpan, {
			throwOnError: false,
			displayMode: true
		});
		document.getElementById("output").appendChild(mathSpan);
	},
	printErr(errOutput)
	{
	try
	{
		let errData = JSON.parse(errOutput);
		// text += errMessages[language][errData.code] + "\n\n";
		let text = "Syntax error: " + t("errors.syntax." + errData.code);
		document.getElementById("errorType").textContent = text;
		text = errData.input + '\n';
		for (let i = 0; i < errData.index; i++)
		{
			text += ' ';
		}
		text += "↑" + "\n";
		document.getElementById("errorDetails").textContent = text;
	}
	catch (e)
	{
		console.error("Invalid JSON:", errOutput);
		return;
	}	
	}
};

function solve()
{
	document.getElementById("output").innerHTML = "";
	document.getElementById("errorType").innerHTML = "";
	document.getElementById("errorDetails").innerHTML = "";
	var expr = document.getElementById("expr").value;
	let ptr = Module.ccall('manage_input', "number", ['string'], [expr]);
	console.log(ptr);
	let aux = UTF8ToString(ptr);
	Module._free(ptr);
	json = JSON.parse(aux);
	if (json.status === "syntax_error")
		Module.printErr(aux);

}

document.getElementById("inputForm").addEventListener("submit", function(event)
{
	event.preventDefault(); // evita que recargue la página
	solve();
});

