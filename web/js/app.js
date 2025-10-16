const EPS = 1e-9;
let ultimoReporte = null;

function limitarDecimales(valor) {
  const numero = Number(valor);
  if (!Number.isFinite(numero)) {
    return 0;
  }
  const ajustado = Number.parseFloat(numero.toFixed(2));
  return Object.is(ajustado, -0) ? 0 : ajustado;
}

function formatearNumero(valor) {
  const ajustado = limitarDecimales(valor);
  return Number.isFinite(ajustado) ? ajustado.toString() : String(valor);
}

document.addEventListener("DOMContentLoaded", () => {
  inicializarTema();
  inicializarEventos();
  construirTabla(parseInt(document.getElementById("dimension").value, 10));
  cargarValoresDemo();
});

function inicializarTema() {
  const toggle = document.getElementById("toggle-tema");
  if (!toggle) return;
  const icono = toggle.querySelector(".icono-tema");
  const raiz = document.documentElement;
  const aplicarTema = (modo, persistir = true) => {
    if (modo === "claro") {
      raiz.setAttribute("data-tema", "claro");
      toggle.setAttribute("aria-label", "Cambiar a modo oscuro");
      if (icono) icono.textContent = String.fromCodePoint(0x1F31E);
      if (persistir) localStorage.setItem("tema-preferido", "claro");
    } else {
      raiz.removeAttribute("data-tema");
      toggle.setAttribute("aria-label", "Cambiar a modo claro");
      if (icono) icono.textContent = String.fromCodePoint(0x1F319);
      if (persistir) localStorage.setItem("tema-preferido", "oscuro");
    }
  };

  const estadoInicial = localStorage.getItem("tema-preferido") === "claro" ? "claro" : "oscuro";
  aplicarTema(estadoInicial, false);

  toggle.addEventListener("click", () => {
    const proximoTema = raiz.getAttribute("data-tema") === "claro" ? "oscuro" : "claro";
    aplicarTema(proximoTema);
  });
}

function inicializarEventos() {
  document.getElementById("dimension").addEventListener("change", (e) => {
    const valor = parseInt(e.target.value, 10);
    const n = Number.isNaN(valor) ? 3 : Math.max(2, Math.min(6, valor));
    e.target.value = n;
    construirTabla(n);
    if (n === 3) {
      cargarValoresDemo();
    } else {
      limpiarAlertas();
      mostrarAlerta(`Se configuro una matriz ${n}x${n + 1}.`, "info");
    }
  });

  document.getElementById("cargar-demo").addEventListener("click", cargarValoresDemo);
  document.getElementById("btn-gauss").addEventListener("click", ejecutarGauss);
  document.getElementById("btn-rref").addEventListener("click", ejecutarGaussJordan);
  document.getElementById("btn-determinante").addEventListener("click", ejecutarDeterminante);
  document.getElementById("btn-exportar-txt").addEventListener("click", exportarTxt);
}

function construirTabla(n) {
  const contenedor = document.getElementById("contenedor-matriz");
  contenedor.innerHTML = "";
  const tabla = document.createElement("table");
  const encabezado = document.createElement("thead");
  const filaHead = document.createElement("tr");

  for (let j = 0; j < n; j++) {
    const th = document.createElement("th");
    th.textContent = `ai${j + 1}`;
    filaHead.appendChild(th);
  }
  const thB = document.createElement("th");
  thB.textContent = "bi";
  filaHead.appendChild(thB);
  encabezado.appendChild(filaHead);
  tabla.appendChild(encabezado);

  const cuerpo = document.createElement("tbody");
  for (let i = 0; i < n; i++) {
    const fila = document.createElement("tr");
    for (let j = 0; j < n + 1; j++) {
      const celda = document.createElement("td");
      const input = document.createElement("input");
      input.type = "number";
      input.step = "any";
      input.setAttribute("aria-label", j < n ? `a[${i + 1},${j + 1}]` : `b[${i + 1}]`);
      input.dataset.fila = i;
      input.dataset.columna = j;
      fila.appendChild(celda);
      celda.appendChild(input);
    }
    cuerpo.appendChild(fila);
  }
  tabla.appendChild(cuerpo);
  contenedor.appendChild(tabla);
}

function cargarValoresDemo() {
  const valores = [
    [1, -1, 2, 9],
    [0, 1, -3, -10],
    [4, 1, 1, 10],
  ];
  const inputs = document.querySelectorAll("#contenedor-matriz input");
  inputs.forEach((input) => {
    const fila = parseInt(input.dataset.fila, 10);
    const col = parseInt(input.dataset.columna, 10);
    if (valores[fila] && typeof valores[fila][col] !== "undefined") {
      input.value = valores[fila][col];
    } else {
      input.value = "";
    }
  });
  mostrarAlerta("Se cargo el caso de demostracion 3x3 con solucion (2, -1, 3).", "info");
}

function obtenerMatrizAumentada() {
  const filas = [];
  const filasDom = document.querySelectorAll("#contenedor-matriz tbody tr");
  for (const filaDom of filasDom) {
    const celdas = [];
    const inputs = filaDom.querySelectorAll("input");
    for (const input of inputs) {
      const valor = parseFloat(input.value);
      if (!Number.isFinite(valor)) {
        input.focus();
        mostrarAlerta("Hay valores vacios o invalidos. Complete todos los campos numericamente.", "error");
        return null;
      }
      celdas.push(valor);
    }
    filas.push(celdas);
  }
  return filas;
}

function clonarMatriz(matriz) {
  return matriz.map((fila) => fila.slice());
}

function esCasiCero(valor) {
  return Math.abs(valor) <= EPS;
}

function limpiarAlertas() {
  const alerta = document.getElementById("alertas");
  alerta.textContent = "";
  alerta.classList.remove("mostrar");
}

function mostrarAlerta(mensaje, tipo = "info") {
  const alerta = document.getElementById("alertas");
  alerta.textContent = mensaje;
  alerta.className = "alertas mostrar";
  if (tipo === "error") {
    alerta.style.borderColor = "rgba(248, 113, 113, 0.6)";
  } else {
    alerta.style.borderColor = "rgba(56, 189, 248, 0.4)";
  }
  setTimeout(() => alerta.classList.remove("mostrar"), 4500);
}

function ejecutarGauss() {
  const matriz = obtenerMatrizAumentada();
  if (!matriz) return;
  const original = clonarMatriz(matriz);
  const pasos = [];
  registrarPaso("Matriz inicial (Gauss)", matriz, pasos);
  const n = matriz.length;
  const columnas = matriz[0].length;
  for (let col = 0; col < n; col++) {
    let filaPivote = col;
    let maxValor = Math.abs(matriz[col][col]);
    for (let fila = col + 1; fila < n; fila++) {
      const valor = Math.abs(matriz[fila][col]);
      if (valor > maxValor) {
        maxValor = valor;
        filaPivote = fila;
      }
    }
    if (esCasiCero(maxValor)) {
      registrarPaso(`Columna ${col + 1} sin pivote util, se omite.`, matriz, pasos);
      continue;
    }
    if (filaPivote !== col) {
      [matriz[filaPivote], matriz[col]] = [matriz[col], matriz[filaPivote]];
      registrarPaso(
        `Intercambio de filas: F${col + 1} <-> F${filaPivote + 1} para seleccionar pivote.`,
        matriz,
        pasos,
        { fila: col, col }
      );
    } else {
      registrarPaso(
        `Pivote seleccionado en F${col + 1}, columna ${col + 1}.`,
        matriz,
        pasos,
        { fila: col, col }
      );
    }
    for (let fila = col + 1; fila < n; fila++) {
      const divisor = matriz[col][col];
      if (esCasiCero(divisor)) continue;
      const factor = matriz[fila][col] / divisor;
      if (esCasiCero(factor)) continue;
      for (let j = col; j < columnas; j++) {
        matriz[fila][j] -= factor * matriz[col][j];
        if (esCasiCero(matriz[fila][j])) matriz[fila][j] = 0;
      }
      registrarPaso(
        `F${fila + 1} <- F${fila + 1} - (${formatearNumero(factor)}) * F${col + 1}`,
        matriz,
        pasos,
        { fila, col }
      );
    }
  }
  registrarPaso("Matriz escalonada final (Gauss).", matriz, pasos);
  const resultado = analizarSistema(matriz, "gauss");
  renderizarPasos(pasos, resultado);
  ultimoReporte = {
    tipo: "gauss",
    matrizOriginal: original,
    pasos,
    resultado,
    timestamp: new Date().toISOString(),
  };
  limpiarLaplace();
  mostrarAlerta(`Procedimiento de Gauss completado. Tipo de sistema: ${resultado.etiqueta}.`, "info");
}

function ejecutarGaussJordan() {
  const matriz = obtenerMatrizAumentada();
  if (!matriz) return;
  const original = clonarMatriz(matriz);
  const pasos = [];
  registrarPaso("Matriz inicial (Gauss-Jordan)", matriz, pasos);
  const n = matriz.length;
  const columnas = matriz[0].length;
  let filaActual = 0;
  for (let col = 0; col < columnas - 1 && filaActual < n; col++) {
    let filaPivote = filaActual;
    let maxValor = Math.abs(matriz[filaActual][col]);
    for (let fila = filaActual + 1; fila < n; fila++) {
      const valor = Math.abs(matriz[fila][col]);
      if (valor > maxValor) {
        maxValor = valor;
        filaPivote = fila;
      }
    }
    if (esCasiCero(maxValor)) continue;
    if (filaPivote !== filaActual) {
      [matriz[filaPivote], matriz[filaActual]] = [matriz[filaActual], matriz[filaPivote]];
      registrarPaso(
        `Se intercambian filas F${filaActual + 1} y F${filaPivote + 1} para elegir el pivote.`,
        matriz,
        pasos,
        { fila: filaActual, col }
      );
    }
    const pivote = matriz[filaActual][col];
    if (esCasiCero(pivote)) continue;
    for (let j = col; j < columnas; j++) {
      matriz[filaActual][j] /= pivote;
      if (esCasiCero(matriz[filaActual][j])) matriz[filaActual][j] = 0;
    }
    registrarPaso(
      `F${filaActual + 1} se divide por ${formatearNumero(pivote)} para normalizar el pivote.`,
      matriz,
      pasos,
      { fila: filaActual, col }
    );
    for (let fila = 0; fila < n; fila++) {
      if (fila === filaActual) continue;
      const factor = matriz[fila][col];
      if (esCasiCero(factor)) continue;
      for (let j = col; j < columnas; j++) {
        matriz[fila][j] -= factor * matriz[filaActual][j];
        if (esCasiCero(matriz[fila][j])) matriz[fila][j] = 0;
      }
      registrarPaso(
        `F${fila + 1} <- F${fila + 1} - (${formatearNumero(factor)}) * F${filaActual + 1}`,
        matriz,
        pasos,
        { fila, col }
      );
    }
    filaActual++;
  }
  registrarPaso("Matriz reducida (RREF).", matriz, pasos);
  const resultado = analizarSistema(matriz, "gauss-jordan");
  renderizarPasos(pasos, resultado);
  ultimoReporte = {
    tipo: "gauss-jordan",
    matrizOriginal: original,
    pasos,
    resultado,
    timestamp: new Date().toISOString(),
  };
  limpiarLaplace();
  mostrarAlerta(`Gauss-Jordan finalizado. Tipo de sistema: ${resultado.etiqueta}.`, "info");
}

function registrarPaso(descripcion, matriz, pasos, pivote = null) {
  pasos.push({
    descripcion,
    matriz: clonarMatriz(matriz),
    pivote,
  });
}

function analizarSistema(matriz, metodo) {
  const n = matriz.length;
  const columnas = matriz[0].length;
  let rangoCoef = 0;
  let rangoAum = 0;
  for (let i = 0; i < n; i++) {
    let tieneCoef = false;
    let tieneAum = false;
    for (let j = 0; j < columnas - 1; j++) {
      if (!esCasiCero(matriz[i][j])) {
        tieneCoef = true;
        tieneAum = true;
      }
    }
    if (!esCasiCero(matriz[i][columnas - 1])) {
      tieneAum = true;
    }
    if (tieneCoef) rangoCoef++;
    if (tieneAum) rangoAum++;
  }
  if (rangoCoef !== rangoAum) {
    return {
      tieneSolucion: false,
      infinitas: false,
      soluciones: [],
      etiqueta: "SS (Sistema incompatible)",
      metodo,
    };
  }
  if (rangoCoef < n) {
    return {
      tieneSolucion: true,
      infinitas: true,
      soluciones: [],
      etiqueta: "CI (Sistema compatible indeterminado)",
      metodo,
    };
  }
  const soluciones = metodo === "gauss-jordan" ? new Array(n).fill(0) : new Array(n).fill(0);
  if (metodo === "gauss-jordan") {
    for (let i = 0; i < n; i++) {
      soluciones[i] = matriz[i][columnas - 1];
    }
  } else {
    for (let i = n - 1; i >= 0; i--) {
      let suma = matriz[i][columnas - 1];
      for (let j = i + 1; j < n; j++) {
        suma -= matriz[i][j] * soluciones[j];
      }
      const pivote = matriz[i][i];
      if (esCasiCero(pivote)) {
        return {
          tieneSolucion: false,
          infinitas: false,
          soluciones: [],
          etiqueta: "SS (Sistema incompatible)",
          metodo,
        };
      }
      soluciones[i] = suma / pivote;
    }
  }
  const solucionesRedondeadas = soluciones.map((valor) => limitarDecimales(valor));
  return {
    tieneSolucion: true,
    infinitas: false,
    soluciones: solucionesRedondeadas,
    etiqueta: "CD (Sistema compatible determinado)",
    metodo,
  };
}

function renderizarPasos(pasos, resultado) {
  const panel = document.getElementById("panel-pasos");
  panel.innerHTML = "";
  if (!pasos.length) {
    panel.textContent = "No hay pasos para mostrar.";
    return;
  }
  pasos.forEach((paso, indice) => {
    const tarjeta = document.createElement("article");
    tarjeta.className = "paso";
    const titulo = document.createElement("div");
    titulo.className = "paso__titulo";
    titulo.textContent = `Paso ${indice + 1}`;
    const descripcion = document.createElement("p");
    descripcion.className = "paso__descripcion";
    descripcion.textContent = paso.descripcion;
    tarjeta.appendChild(titulo);
    tarjeta.appendChild(descripcion);
    if (paso.matriz) {
      const envoltorio = document.createElement("div");
      envoltorio.className = "paso__matriz";
      envoltorio.appendChild(crearTablaDesdeMatriz(paso.matriz, paso.pivote));
      tarjeta.appendChild(envoltorio);
    }
    panel.appendChild(tarjeta);
  });

  const resumen = document.createElement("div");
  resumen.className = "mensaje-resultado";
  resumen.textContent = `Resultado: ${resultado.etiqueta}`;
  panel.appendChild(resumen);
  if (resultado.tieneSolucion && !resultado.infinitas) {
    const detalle = document.createElement("p");
    detalle.textContent = `Solucion aproximada: [ ${resultado.soluciones
      .map((x) => formatearNumero(x))
      .join(", ")} ]`;
    panel.appendChild(detalle);
  } else if (!resultado.tieneSolucion) {
    const detalle = document.createElement("p");
    detalle.textContent = "El sistema no posee solucion real.";
    panel.appendChild(detalle);
  } else {
    const detalle = document.createElement("p");
    detalle.textContent = "Existen infinitas soluciones (se recomienda parametrizar manualmente).";
    panel.appendChild(detalle);
  }
}

function crearTablaDesdeMatriz(matriz, pivote) {
  const tabla = document.createElement("table");
  matriz.forEach((fila, i) => {
    const tr = document.createElement("tr");
    fila.forEach((valor, j) => {
      const td = document.createElement("td");
      td.textContent = formatearNumero(valor);
      if (pivote && pivote.fila === i && pivote.col === j) {
        td.classList.add("resaltar");
      }
      tr.appendChild(td);
    });
    tabla.appendChild(tr);
  });
  return tabla;
}

function ejecutarDeterminante() {
  const matriz = obtenerMatrizAumentada();
  if (!matriz) return;
  const cuadrada = matriz.map((fila) => fila.slice(0, matriz.length));
  const pasos = [];
  const n = cuadrada.length;
  if (n >= 6) {
    mostrarAlerta("Atencion: Laplace recursiva puede tardar para n grandes. Se continuara igualmente.", "info");
  }
  let valor = 0;
  let metodo = "";
  let arbol = null;
  if (n === 2) {
    metodo = "Formula directa 2x2";
    const diag = cuadrada[0][0] * cuadrada[1][1];
    const anti = cuadrada[0][1] * cuadrada[1][0];
    valor = diag - anti;
    pasos.push(
      `Determinante 2x2: (${formatearNumero(cuadrada[0][0])} * ${formatearNumero(
        cuadrada[1][1]
      )}) - (${formatearNumero(cuadrada[0][1])} * ${formatearNumero(cuadrada[1][0])}) = ${formatearNumero(valor)}`
    );
  } else if (n === 3) {
    metodo = "Regla de Sarrus";
    const principal =
      cuadrada[0][0] * cuadrada[1][1] * cuadrada[2][2] +
      cuadrada[0][1] * cuadrada[1][2] * cuadrada[2][0] +
      cuadrada[0][2] * cuadrada[1][0] * cuadrada[2][1];
    const secundaria =
      cuadrada[0][2] * cuadrada[1][1] * cuadrada[2][0] +
      cuadrada[0][0] * cuadrada[1][2] * cuadrada[2][1] +
      cuadrada[0][1] * cuadrada[1][0] * cuadrada[2][2];
    valor = principal - secundaria;
    pasos.push(`Suma de diagonales principales: ${formatearNumero(principal)}`);
    pasos.push(`Suma de diagonales secundarias: ${formatearNumero(secundaria)}`);
    pasos.push(`Determinante 3x3 = ${formatearNumero(valor)}`);
  } else {
    metodo = "Expansion de Laplace (fila 1)";
    const resultadoLaplace = laplaceRecursiva(cuadrada, pasos, 0);
    valor = resultadoLaplace.valor;
    arbol = resultadoLaplace.nodo;
    pasos.push(`Determinante final = ${formatearNumero(valor)}`);
  }
  renderizarDeterminante(pasos, valor, metodo, arbol);
  ultimoReporte = {
    tipo: "determinante",
    matrizOriginal: cuadrada,
    pasos,
    metodo,
    valor,
    arbol,
    timestamp: new Date().toISOString(),
  };
  mostrarAlerta(`Determinante calculada con ${metodo}: ${formatearNumero(valor)}.`, "info");
}

function laplaceRecursiva(matriz, pasos, profundidad) {
  const n = matriz.length;
  const indent = "  ".repeat(profundidad);
  if (n === 1) {
    const valor = matriz[0][0];
    pasos.push(`${indent}Base 1x1 -> det = ${formatearNumero(valor)}`);
    return {
      valor,
      nodo: {
        resumen: `Base 1x1 = ${formatearNumero(valor)}`,
        detalle: "Valor directo (unico elemento).",
        matriz: matriz,
        hijos: [],
      },
    };
  }
  if (n === 2) {
    const valor = matriz[0][0] * matriz[1][1] - matriz[0][1] * matriz[1][0];
    pasos.push(`${indent}Determinante 2x2 auxiliar = ${formatearNumero(valor)}`);
    return {
      valor,
      nodo: {
        resumen: `Det 2x2 = ${formatearNumero(valor)}`,
        detalle: "Aplicacion directa de ad - bc.",
        matriz: matriz,
        hijos: [],
      },
    };
  }
  let suma = 0;
  const hijos = [];
  for (let col = 0; col < n; col++) {
    const elemento = matriz[0][col];
    if (esCasiCero(elemento)) continue;
    const signo = col % 2 === 0 ? 1 : -1;
    pasos.push(
      `${indent}Elemento a[1,${col + 1}] = ${formatearNumero(elemento)} con signo ${
        signo > 0 ? "+" : "-"
      }`
    );
    const menor = construirMenor(matriz, col);
    const resultadoMenor = laplaceRecursiva(menor, pasos, profundidad + 1);
    const aporte = signo * elemento * resultadoMenor.valor;
    pasos.push(
      `${indent}Aporte: ${signo} * ${formatearNumero(elemento)} * ${formatearNumero(
        resultadoMenor.valor
      )} = ${formatearNumero(aporte)}`
    );
    suma += aporte;
    hijos.push({
      resumen: `a[1,${col + 1}] = ${formatearNumero(elemento)} (signo ${signo > 0 ? "+" : "-"})`,
      detalle: `Det(menor) = ${formatearNumero(resultadoMenor.valor)} -> aporte = ${formatearNumero(
        aporte
      )}`,
      matriz: menor,
      hijos: resultadoMenor.nodo ? [resultadoMenor.nodo] : [],
    });
  }
  return {
    valor: suma,
    nodo: {
      resumen: `Nivel ${profundidad + 1}: det = ${formatearNumero(suma)}`,
      detalle: `Suma de ${hijos.length} cofactores.`,
      matriz,
      hijos,
    },
  };
}

function construirMenor(matriz, columnaEliminada) {
  const menor = [];
  for (let i = 1; i < matriz.length; i++) {
    const filaMenor = [];
    for (let j = 0; j < matriz.length; j++) {
      if (j === columnaEliminada) continue;
      filaMenor.push(matriz[i][j]);
    }
    menor.push(filaMenor);
  }
  return menor;
}

function renderizarDeterminante(pasos, valor, metodo, arbol) {
  const panel = document.getElementById("panel-pasos");
  panel.innerHTML = "";
  pasos.forEach((linea, indice) => {
    const tarjeta = document.createElement("article");
    tarjeta.className = "paso";
    const titulo = document.createElement("div");
    titulo.className = "paso__titulo";
    titulo.textContent = `Paso ${indice + 1}`;
    const descripcion = document.createElement("p");
    descripcion.className = "paso__descripcion";
    descripcion.textContent = linea;
    tarjeta.appendChild(titulo);
    tarjeta.appendChild(descripcion);
    panel.appendChild(tarjeta);
  });
  const resumen = document.createElement("div");
  resumen.className = "mensaje-resultado";
  resumen.textContent = `Determinante (${metodo}): ${formatearNumero(valor)}`;
  panel.appendChild(resumen);
  if (arbol) {
    construirArbolLaplace(arbol);
  } else {
    limpiarLaplace();
  }
}

function construirArbolLaplace(nodo) {
  const contenedor = document.getElementById("laplace-contenedor");
  contenedor.innerHTML = "";
  const elemento = crearNodoLaplace(nodo);
  contenedor.appendChild(elemento);
}

function crearNodoLaplace(nodo) {
  const details = document.createElement("details");
  details.open = nodo.matriz.length <= 3;
  const summary = document.createElement("summary");
  summary.textContent = nodo.resumen;
  details.appendChild(summary);
  const contenido = document.createElement("div");
  contenido.className = "laplace__contenido";
  const descripcion = document.createElement("p");
  descripcion.textContent = nodo.detalle;
  contenido.appendChild(descripcion);
  contenido.appendChild(crearTablaDesdeMatriz(nodo.matriz));
  if (nodo.hijos && nodo.hijos.length) {
    nodo.hijos.forEach((hijo) => contenido.appendChild(crearNodoLaplace(hijo)));
  }
  details.appendChild(contenido);
  return details;
}

function limpiarLaplace() {
  document.getElementById("laplace-contenedor").innerHTML = "";
}

function exportarTxt() {
  if (!ultimoReporte) {
    mostrarAlerta("No hay resultados para exportar. Ejecute un calculo primero.", "error");
    return;
  }
  const contenido = generarTextoReporte(ultimoReporte);
  const enlace = document.createElement("a");
  enlace.href = URL.createObjectURL(new Blob([contenido], { type: "text/plain" }));
  enlace.download = `reporte_${ultimoReporte.tipo}_${formatearTimestamp(ultimoReporte.timestamp)}.txt`;
  enlace.click();
  mostrarAlerta("Reporte .txt descargado.", "info");
}


function generarTextoReporte(reporte) {
  const lineas = [];
  lineas.push("=== REPORTE AUTOMATICO ===");
  lineas.push(`Tipo: ${reporte.tipo}`);
  lineas.push(`Fecha: ${reporte.timestamp}`);
  if (reporte.tipo === "determinante") {
    lineas.push(`Metodo: ${reporte.metodo}`);
    lineas.push(`Valor: ${formatearNumero(reporte.valor)}`);
    lineas.push("--- Pasos ---");
    reporte.pasos.forEach((paso, idx) => lineas.push(`Paso ${idx + 1}: ${paso}`));
  } else {
    lineas.push(`Clasificacion: ${reporte.resultado.etiqueta}`);
    if (reporte.resultado.tieneSolucion && !reporte.resultado.infinitas) {
      lineas.push(
        `Solucion: [ ${reporte.resultado.soluciones.map((x) => formatearNumero(x)).join(", ")} ]`
      );
    }
    lineas.push("--- Pasos ---");
    reporte.pasos.forEach((paso, idx) => {
      lineas.push(`Paso ${idx + 1}: ${paso.descripcion}`);
      paso.matriz.forEach((fila) =>
        lineas.push(`   ${fila.map((v) => formatearNumero(v)).join("\t")}`)
      );
    });
  }
  return lineas.join("\n");
}

function formatearTimestamp(iso) {
  const fecha = new Date(iso);
  const pad = (n) => n.toString().padStart(2, "0");
  return `${fecha.getFullYear()}${pad(fecha.getMonth() + 1)}${pad(fecha.getDate())}_${pad(
    fecha.getHours()
  )}${pad(fecha.getMinutes())}${pad(fecha.getSeconds())}`;
}





