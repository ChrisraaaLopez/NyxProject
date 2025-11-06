// Constantes del DOM
const btnTakePhoto = document.getElementById("btnTakePhoto");
const btnCapture = document.getElementById("btnCapture");
const btnCancel = document.getElementById("btnCancel");
const btnSave = document.getElementById("btnSave");
const videoModal = document.getElementById("videoModal");
const video = document.getElementById("video");
const canvas = document.getElementById("canvas");
const preview = document.getElementById("preview");
const cameraIcon = document.getElementById("cameraIcon");
const backgroundParticles = document.getElementById("backgroundParticles");

let stream = null;

// Función para mostrar mensajes de sistema (éxito/error) en la interfaz
function showSystemMessage(message, isSuccess = false) {
    document.querySelectorAll(".system-message").forEach(el => el.remove());

    const messageDiv = document.createElement("div");
    messageDiv.classList.add("system-message");
    if (isSuccess) {
        messageDiv.classList.add("success");
    }
    messageDiv.textContent = message;

    document.body.appendChild(messageDiv);

    // El mensaje desaparece después de 4 segundos
    setTimeout(() => {
        messageDiv.style.opacity = 0;
        setTimeout(() => messageDiv.remove(), 500);
    }, 4000);
}

// Evento para solicitar acceso a la cámara y mostrar el modal de video
btnTakePhoto.addEventListener("click", async() => {
    try {
        // Solicitar acceso a la cámara (requiere HTTPS o localhost)
        stream = await navigator.mediaDevices.getUserMedia({
            video: true
        });
        video.srcObject = stream;
        videoModal.classList.add("show");
    } catch (err) {
        // Muestra el tipo de error (ej: NotAllowedError) para facilitar el diagnóstico
        showSystemMessage(`🚨 Error (${err.name}): ${err.message}. Verifique permisos y conexión.`, false);
    }
});

// Evento para tomar la foto desde el video stream y mostrarla en la vista previa
btnCapture.addEventListener("click", () => {
    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;
    const ctx = canvas.getContext("2d");
    ctx.drawImage(video, 0, 0, canvas.width, canvas.height);

    const imageDataUrl = canvas.toDataURL("image/png");
    preview.src = imageDataUrl;
    preview.style.display = "block";
    cameraIcon.style.display = "none";

    closeCamera();
});

// Evento para cerrar la cámara si se cancela la captura
btnCancel.addEventListener("click", () => {
    closeCamera();
});

// Función para detener el stream de la cámara y ocultar el modal
function closeCamera() {
    if (stream) {
        stream.getTracks().forEach((track) => track.stop());
        stream = null;
    }
    videoModal.classList.remove("show");
}

// Evento para validar datos, simular el guardado y limpiar el formulario
btnSave.addEventListener("click", () => {
    const data = {
        nombreCompleto: document.getElementById("nombreCompleto").value,
        apellido1: document.getElementById("apellido1").value,
        apellido2: document.getElementById("apellido2").value,
        curp: document.getElementById("curp").value,
        foto: preview.src,
    };

    const inputs = [data.nombreCompleto, data.apellido1, data.apellido2, data.curp];

    // Validación de campos vacíos
    if (inputs.some(val => !val)) {
        showSystemMessage("🚨 Por favor, complete todos los campos requeridos.", false);
        return;
    }

    // Validación de la foto
    if (preview.style.display === "none" || !preview.src || preview.src.includes("data:,")) {
        showSystemMessage("📸 Por favor, tome una foto antes de guardar.", false);
        return;
    }

    // Simulación de envío/guardado
    console.log("Datos guardados:", data);
    showSystemMessage("✅ Datos guardados correctamente en el registro.", true);

    // Limpiar formulario y restablecer vista previa
    document.getElementById("nombreCompleto").value = "";
    document.getElementById("apellido1").value = "";
    document.getElementById("apellido2").value = "";
    document.getElementById("curp").value = "";

    preview.style.display = "none";
    preview.src = "";
    cameraIcon.style.display = "block";
});

// Configuración y función de generación optimizada para las partículas de fondo
const NUM_PARTICLES = 50;

function createParticle() {
    const particle = document.createElement("div");
    particle.classList.add("particle");
    backgroundParticles.appendChild(particle);

    const size = Math.random() * 2 + 1;
    particle.style.width = `${size}px`;
    particle.style.height = `${size}px`;

    const startX = Math.random() * window.innerWidth;
    const startY = Math.random() * window.innerHeight;
    particle.style.left = `${startX}px`;
    particle.style.top = `${startY}px`;

    const moveX = (Math.random() - 0.5) * 100;
    const moveY = (Math.random() - 0.5) * 100;

    particle.style.setProperty("--x", `${moveX}px`);
    particle.style.setProperty("--y", `${moveY}px`);

    const duration = Math.random() * 6 + 4;
    const delay = Math.random() * 4;
    particle.style.animationDuration = `${duration}s`;
    particle.style.animationDelay = `${delay}s`;

    // Regeneración automática para mantener la densidad fija
    particle.addEventListener("animationend", () => {
        particle.remove();
        if (backgroundParticles.children.length < NUM_PARTICLES) {
            createParticle();
        }
    });
}

// Inicializa el número fijo de partículas
for (let i = 0; i < NUM_PARTICLES; i++) {
    createParticle();
}