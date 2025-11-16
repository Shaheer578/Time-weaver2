// Global state
let currentUsername = "";
let currentUserRole = "";
let currentUserDisplayName = ""; // Store actual name for students/teachers
let currentUserUniversity = "";
let currentUserDepartment = ""; // Department code such as CT, ME, EE, CHE
let currentSelectedDate = new Date();
let currentlyEditingEventId = -1;
let currentMonth = new Date().getMonth() + 1;
let currentYear = new Date().getFullYear();

// API base URL - automatically detects environment
const API_BASE = window.location.hostname === 'localhost' || window.location.hostname === '127.0.0.1' 
  ? "http://localhost:8080/api" 
  : "/api";

// Month logos (SVG data)
const monthLogos = {
  1: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#E8F4F8"/><path d="M32 12 L36 20 L44 22 L38 30 L40 38 L32 34 L24 38 L26 30 L20 22 L28 20 Z" fill="#4FC3F7" stroke="#0277BD" stroke-width="2"/></svg>`,
  2: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FCE4EC"/><path d="M32 18 C32 18 24 24 24 32 C24 38 28 42 32 42 C36 42 40 38 40 32 C40 24 32 18 32 18 Z" fill="#F06292" stroke="#C2185B" stroke-width="2"/><circle cx="28" cy="30" r="2" fill="#880E4F"/><circle cx="36" cy="30" r="2" fill="#880E4F"/></svg>`,
  3: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#F1F8E9"/><path d="M32 20 L28 28 L32 30 L28 38 L36 30 L32 28 L36 20 Z" fill="#AED581" stroke="#558B2F" stroke-width="2"/><circle cx="32" cy="40" r="3" fill="#FF6F00"/><circle cx="26" cy="36" r="2" fill="#FF6F00"/><circle cx="38" cy="36" r="2" fill="#FF6F00"/></svg>`,
  4: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#E1F5FE"/><path d="M20 38 Q26 30 32 35 Q38 30 44 38" fill="none" stroke="#29B6F6" stroke-width="3" stroke-linecap="round"/><path d="M24 22 L26 28 L32 26 L28 32" fill="#81D4FA"/><path d="M40 22 L38 28 L32 26 L36 32" fill="#81D4FA"/><circle cx="32" cy="42" r="4" fill="#4FC3F7"/></svg>`,
  5: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FFF9C4"/><circle cx="32" cy="24" r="8" fill="#FFEB3B" stroke="#F57F17" stroke-width="2"/><line x1="32" y1="16" x2="32" y2="12" stroke="#F57F17" stroke-width="2"/><line x1="32" y1="32" x2="32" y2="36" stroke="#F57F17" stroke-width="2"/><line x1="24" y1="24" x2="20" y2="24" stroke="#F57F17" stroke-width="2"/><line x1="40" y1="24" x2="44" y2="24" stroke="#F57F17" stroke-width="2"/><path d="M22 40 Q27 36 32 38 Q37 36 42 40" fill="#FDD835"/></svg>`,
  6: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FFF3E0"/><circle cx="32" cy="20" r="6" fill="#FFB74D"/><path d="M32 26 L32 38" stroke="#E65100" stroke-width="2"/><ellipse cx="32" cy="42" rx="8" ry="6" fill="#FF9800"/><path d="M26 38 L32 40 L38 38" fill="#FFB74D"/></svg>`,
  7: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FFEBEE"/><circle cx="32" cy="22" r="10" fill="#EF5350" stroke="#C62828" stroke-width="2"/><line x1="28" y1="18" x2="28" y2="26" stroke="#C62828" stroke-width="2"/><line x1="36" y1="18" x2="36" y2="26" stroke="#C62828" stroke-width="2"/><path d="M24 36 Q28 40 32 38 Q36 40 40 36" stroke="#E53935" stroke-width="2" fill="none"/></svg>`,
  8: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#E0F2F1"/><path d="M32 18 L28 26 L20 28 L26 34 L24 42 L32 38 L40 42 L38 34 L44 28 L36 26 Z" fill="#26A69A" stroke="#00695C" stroke-width="2"/><circle cx="32" cy="32" r="4" fill="#80CBC4"/></svg>`,
  9: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FBE9E7"/><path d="M28 20 L32 16 L36 20 L36 28 L28 28 Z" fill="#FF8A65" stroke="#D84315" stroke-width="2"/><rect x="26" y="28" width="12" height="16" rx="2" fill="#FFAB91" stroke="#D84315" stroke-width="2"/></svg>`,
  10: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#FFF8E1"/><path d="M32 16 C32 16 28 20 26 26 L32 28 L26 36 C28 40 32 42 32 42 C32 42 36 40 38 36 L32 28 L38 26 C36 20 32 16 32 16 Z" fill="#FFB300" stroke="#F57C00" stroke-width="2"/></svg>`,
  11: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#EFEBE9"/><path d="M20 34 Q24 28 28 32 L30 28 L34 32 L36 28 Q40 28 44 34" fill="none" stroke="#A1887F" stroke-width="3" stroke-linecap="round"/><circle cx="28" cy="22" r="3" fill="#8D6E63"/><circle cx="36" cy="22" r="3" fill="#8D6E63"/><path d="M26 38 L32 42 L38 38" fill="#BCAAA4"/></svg>`,
  12: `<svg viewBox="0 0 64 64" fill="none" xmlns="http://www.w3.org/2000/svg"><circle cx="32" cy="32" r="28" fill="#E3F2FD"/><path d="M32 16 L28 22 L32 24 L28 30 L32 32 L28 38 M32 16 L36 22 L32 24 L36 30 L32 32 L36 38" stroke="#42A5F5" stroke-width="2" fill="none"/><circle cx="24" cy="42" r="3" fill="#90CAF9"/><circle cx="32" cy="44" r="3" fill="#90CAF9"/><circle cx="40" cy="42" r="3" fill="#90CAF9"/></svg>`,
};

// Theme management
let currentTheme = localStorage.getItem("theme") || "light";

function setTheme(theme) {
  currentTheme = theme;
  localStorage.setItem("theme", theme);

  // Remove all theme classes
  document.body.classList.remove(
    "theme-light",
    "theme-dark",
    "theme-nature",
    "theme-neon",
    "theme-gray",
  );

  // Add current theme class
  if (theme !== "light") {
    document.body.classList.add(`theme-${theme}`);
  }

  // Update active theme button
  document.querySelectorAll(".theme-btn").forEach((btn) => {
    btn.classList.toggle("active", btn.dataset.theme === theme);
  });
}

// Page navigation
function showPage(pageId) {
  document
    .querySelectorAll(".page")
    .forEach((page) => page.classList.remove("active"));
  document.getElementById(pageId).classList.add("active");
  
  // Add/remove body class for calendar page
  if (pageId === 'calendarPage') {
    document.body.classList.add('calendar-active');
  } else {
    document.body.classList.remove('calendar-active');
  }
}

// Lightweight app modal
function showAppMessage(message) {
  const overlay = document.getElementById("appModalOverlay");
  const box = document.getElementById("appModal");
  const msg = document.getElementById("appModalMessage");
  msg.textContent = message;
  overlay.style.display = "flex";
}

// Confirmation modal with Yes/No buttons
function showConfirmDialog(message, onConfirm, onCancel) {
  const overlay = document.getElementById("appModalOverlay");
  const box = document.getElementById("appModal");
  const msg = document.getElementById("appModalMessage");
  const closeBtn = document.getElementById("appModalClose");
  
  msg.textContent = message;
  
  // Replace OK button with Yes/No buttons
  closeBtn.style.display = "none";
  
  // Create Yes button
  const yesBtn = document.createElement("button");
  yesBtn.textContent = "Yes";
  yesBtn.className = "btn btn-danger";
  yesBtn.style.width = "auto";
  yesBtn.style.marginTop = "12px";
  yesBtn.style.marginRight = "10px";
  yesBtn.onclick = () => {
    overlay.style.display = "none";
    closeBtn.style.display = "block";
    yesBtn.remove();
    noBtn.remove();
    if (onConfirm) onConfirm();
  };
  
  // Create No button
  const noBtn = document.createElement("button");
  noBtn.textContent = "No";
  noBtn.className = "btn btn-secondary";
  noBtn.style.width = "auto";
  noBtn.style.marginTop = "12px";
  noBtn.onclick = () => {
    overlay.style.display = "none";
    closeBtn.style.display = "block";
    yesBtn.remove();
    noBtn.remove();
    if (onCancel) onCancel();
  };
  
  box.appendChild(yesBtn);
  box.appendChild(noBtn);
  overlay.style.display = "flex";
}
function closeAppModal() {
  const overlay = document.getElementById("appModalOverlay");
  if (overlay) overlay.style.display = "none";
}
// Backward-compat: route any alert() usage to our modal and expose openAppModal alias
window.alert = showAppMessage;
function openAppModal(message) {
  showAppMessage(message);
}

async function loadAdminUsers() {
  const res = await fetch(`${API_BASE}/users?admin=true`);
  const data = await res.json();
  const list = document.getElementById("adminUsersList");
  list.innerHTML = "";
  (data.users || []).forEach((u) => {
    const div = document.createElement("div");
    div.className = "event-item";
    div.textContent = `${u.username} (${u.role})`;
    div.title = "Click to delete this user";
    div.style.display = "flex";
    div.style.justifyContent = "space-between";
    div.style.alignItems = "center";
    const del = document.createElement("button");
    del.textContent = "Delete";
    del.className = "btn btn-danger";
    del.style.width = "auto";
    del.style.margin = "0 0 0 10px";
    del.onclick = async (e) => {
      e.stopPropagation();
      showConfirmDialog(
        `Delete user '${u.username}' with role '${u.role}'?`,
        async () => {
          const res = await fetch(
            `${API_BASE}/users?admin=true&username=${encodeURIComponent(u.username)}&role=${encodeURIComponent(u.role)}`,
            { method: "DELETE" },
          );
          const resp = await res.json();
          if (resp.success) {
            showAppMessage("User deleted.");
            loadAdminUsers();
          } else {
            showAppMessage("Failed to delete user.");
          }
        }
      );
    };
    div.appendChild(del);
    // Also allow clicking the row to trigger delete with confirm
    div.onclick = () => del.onclick(new Event("click"));
    list.appendChild(div);
  });
}

document.getElementById("adminTogglePassword").addEventListener("click", () => {
  const input = document.getElementById("adminNewPassword");
  if (input.type === "password") {
    input.type = "text";
    document.getElementById("adminTogglePassword").textContent = "Hide";
  } else {
    input.type = "password";
    document.getElementById("adminTogglePassword").textContent = "Show";
  }
});

document
  .getElementById("adminChangePasswordButton")
  .addEventListener("click", async () => {
    const username = document
      .getElementById("adminTargetUsername")
      .value.trim();
    const role = document.getElementById("adminTargetRole").value.trim();
    const password = document.getElementById("adminNewPassword").value;
    if (!username || !role || !password) {
      alert("Please fill all fields.");
      return;
    }
    const res = await fetch(`${API_BASE}/users/password`, {
      method: "PUT",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ admin: "true", username, role, password }),
    });
    const data = await res.json();
    if (data.success) {
      showAppMessage("Password updated.");
    } else {
      showAppMessage(
        "No such user found for given role. Please check username/role.",
      );
    }
  });

document
  .getElementById("adminDeleteUserButton")
  .addEventListener("click", async () => {
    const username = document
      .getElementById("adminDeleteUsername")
      .value.trim();
    const role = document.getElementById("adminDeleteRole").value.trim();
    if (!username || !role) {
      alert("Provide username and role.");
      return;
    }
    const res = await fetch(
      `${API_BASE}/users?admin=true&username=${encodeURIComponent(username)}&role=${encodeURIComponent(role)}`,
      {
        method: "DELETE",
      },
    );
    const data = await res.json();
    if (data.success) {
      alert("User deleted.");
      loadAdminUsers();
    } else {
      alert("Failed to delete user.");
    }
  });

// Calendar setup
function setupCalendarPage() {
  const displayName = currentUserDisplayName || currentUsername;
  document.getElementById("currentUserLabel").textContent =
    `Welcome, ${displayName}!`;

  // Personal users can filter by religion; academic users cannot
  document.getElementById("religionComboBox").style.display =
    currentUserRole === "personal" ? "block" : "none";
  if (currentUserRole !== "personal") {
    document.getElementById("religionComboBox").selectedIndex = 0;
  }

  const isStudent = currentUserRole === "student";
  const isTeacher = currentUserRole === "teacher";
  const isPersonal = currentUserRole === "personal";

  // Student-specific UI (REMOVED course system)
  const taskManagerBtn = document.getElementById("openTaskManagerButton");
  if (taskManagerBtn) taskManagerBtn.style.display = isStudent ? "block" : "none";

  // Teacher / student academic buttons
  const deptEventBtn = document.getElementById("teacherDepartmentEventButton");
  const availabilityBtn = document.getElementById("teacherAvailabilityButton");
  const findTeacherBtn = document.getElementById("studentFindTeacherButton");
  if (deptEventBtn) deptEventBtn.style.display = isTeacher ? "block" : "none";
  if (availabilityBtn)
    availabilityBtn.style.display = isTeacher ? "block" : "none";
  if (findTeacherBtn)
    findTeacherBtn.style.display = isStudent ? "block" : "none";

  // Quick Add Event Section - Hide for teachers, show for students and personal
  const quickAddSection = document.getElementById("quickAddEventSection");
  if (quickAddSection) {
    quickAddSection.style.display = isTeacher ? "none" : "block";
  }

  currentSelectedDate = new Date();
  currentMonth = currentSelectedDate.getMonth() + 1;
  currentYear = currentSelectedDate.getFullYear();

  showPage("calendarPage");
  updateEventList();
  updateQuote();
  renderCalendar();
  updateSearchSuggestions();
}

// Notepad Functions
let saveTimeout;

function openTaskManager() {
  document.getElementById('taskModalOverlay').classList.add('active');
  loadNotepad();
  updateNotepadStats();
}

function closeTaskManager() {
  document.getElementById('taskModalOverlay').classList.remove('active');
}

function loadNotepad() {
  const saved = localStorage.getItem(`notepad_${currentUsername}`);
  const textarea = document.getElementById('notepadTextarea');
  if (saved) {
    textarea.value = saved;
    const lastSaved = localStorage.getItem(`notepad_${currentUsername}_time`);
    if (lastSaved) {
      document.getElementById('lastSaved').textContent = new Date(lastSaved).toLocaleTimeString();
    }
  }
}

function saveNotepad() {
  const textarea = document.getElementById('notepadTextarea');
  const content = textarea.value;
  localStorage.setItem(`notepad_${currentUsername}`, content);
  localStorage.setItem(`notepad_${currentUsername}_time`, new Date().toISOString());
  document.getElementById('lastSaved').textContent = new Date().toLocaleTimeString();
}

function autoSaveNotepad() {
  clearTimeout(saveTimeout);
  saveTimeout = setTimeout(() => {
    saveNotepad();
  }, 1000);
}

function updateNotepadStats() {
  const textarea = document.getElementById('notepadTextarea');
  const content = textarea.value;
  document.getElementById('charCount').textContent = content.length;
  document.getElementById('lineCount').textContent = content.split('\n').length;
}

function insertText(text) {
  const textarea = document.getElementById('notepadTextarea');
  const start = textarea.selectionStart;
  const end = textarea.selectionEnd;
  const value = textarea.value;
  textarea.value = value.substring(0, start) + text + value.substring(end);
  textarea.selectionStart = textarea.selectionEnd = start + text.length;
  textarea.focus();
  autoSaveNotepad();
  updateNotepadStats();
}

function clearNotepad() {
  showAppMessage('Clear all notes? This cannot be undone.');
  // Add confirmation buttons to modal
  const modal = document.getElementById('appModal');
  const closeBtn = document.getElementById('appModalClose');
  closeBtn.textContent = 'Cancel';
  
  const confirmBtn = document.createElement('button');
  confirmBtn.textContent = 'Clear All';
  confirmBtn.className = 'btn btn-danger';
  confirmBtn.style.width = 'auto';
  confirmBtn.style.marginTop = '12px';
  confirmBtn.style.marginLeft = '10px';
  confirmBtn.onclick = () => {
    document.getElementById('notepadTextarea').value = '';
    saveNotepad();
    updateNotepadStats();
    closeAppModal();
    showAppMessage('Notes cleared successfully!');
  };
  
  closeBtn.parentNode.appendChild(confirmBtn);
  
  // Reset on close
  const originalClose = closeAppModal;
  window.closeAppModal = () => {
    if (confirmBtn.parentNode) confirmBtn.remove();
    closeBtn.textContent = 'OK';
    window.closeAppModal = originalClose;
    originalClose();
  };
}

function downloadNotes() {
  const textarea = document.getElementById('notepadTextarea');
  const content = textarea.value.trim();
  
  if (!content) {
    showAppMessage('Your notepad is empty! Write something first before downloading.');
    return;
  }
  
  const blob = new Blob([content], { type: 'text/plain' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url;
  a.download = `notes_${currentUsername}_${new Date().toISOString().split('T')[0]}.txt`;
  a.click();
  URL.revokeObjectURL(url);
  showAppMessage('Notes downloaded successfully!');
}

// Old task functions (kept for compatibility)
async function refreshTasks() {
  // Not needed anymore - using notepad
}
document
  .getElementById("addTaskButton")
  ?.addEventListener("click", async () => {
    const title = document.getElementById("taskTitleInput").value.trim();
    const due = document.getElementById("taskDueInput").value.trim();
    if (!title) {
      alert("Task title is required");
      return;
    }
    await fetch(`${API_BASE}/tasks`, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ username: currentUsername, title, due }),
    });
    document.getElementById("taskTitleInput").value = "";
    document.getElementById("taskDueInput").value = "";
    refreshTasks();
  });

// Event list update
async function updateEventList() {
  const dateStr = formatDate(currentSelectedDate);
  const response = await fetch(`${API_BASE}/events?username=${currentUsername}&date=${dateStr}`);
  const data = await response.json();

  const eventList = document.getElementById("eventListWidget");
  eventList.innerHTML = "";

  // Format date label
  const options = { weekday: "long", year: "numeric", month: "long", day: "numeric" };
  document.getElementById("selectedDateLabel").textContent = currentSelectedDate.toLocaleDateString("en-US", options);

  // Sort events by time
  let events = data.events || [];
  
  // Filter by category if selected
  const categoryFilter = document.getElementById("categoryFilter")?.value;
  if (categoryFilter) {
    events = events.filter(event => event.category === categoryFilter);
  }

  let selectedEventId = null;

  events.forEach((event) => {
    const item = document.createElement("div");
    item.className = "event-item";
    item.dataset.eventId = event.id;

    let displayText = event.description;
    if (event.startTime) {
      displayText = formatTime(event.startTime) + " " + displayText;
    }

    const prefix = event.isPersonal ? "[P]" : "[A]";
    const categoryIcon = event.categoryIcon || "";
    item.textContent = prefix + " " + (categoryIcon ? categoryIcon + " " : "") + displayText;

    if (event.color) {
      item.style.borderLeftColor = event.color;
      item.style.borderLeftWidth = "4px";
      item.style.borderLeftStyle = "solid";
    }

    item.addEventListener("click", () => {
      document.querySelectorAll(".event-item").forEach((el) => el.classList.remove("selected"));
      item.classList.add("selected");
      selectedEventId = event.id;
      document.getElementById("eventActions").style.display = "block";
      
      // Store event data for editing
      window.selectedEventData = event;
    });

    eventList.appendChild(item);
  });

  // Always add General holidays (Pakistani national events)
  const generalHolidayResponse = await fetch(`${API_BASE}/holidays?date=${dateStr}&religion=General`);
  const generalHolidayData = await generalHolidayResponse.json();
  
  (generalHolidayData.holidays || []).forEach((holiday) => {
    const item = document.createElement("div");
    item.className = "event-item";
    item.textContent = "[H] " + holiday;
    item.style.color = "#006400";
    item.dataset.eventId = -1;
    eventList.appendChild(item);
  });

  // Add holidays if personal calendar and religion is selected
  if (currentUserRole === "personal") {
    const religion = document.getElementById("religionComboBox").value;
    if (religion !== "Select Religion" && religion !== "General") {
      const holidayResponse = await fetch(`${API_BASE}/holidays?date=${dateStr}&religion=${religion}`);
      const holidayData = await holidayResponse.json();

      (holidayData.holidays || []).forEach((holiday) => {
        const item = document.createElement("div");
        item.className = "event-item";
        item.textContent = "[H] " + holiday;
        item.style.color = "#006400";
        item.dataset.eventId = -1;
        eventList.appendChild(item);
      });
    }
  }
  
  // Hide actions if no events
  if (events.length === 0) {
    document.getElementById("eventActions").style.display = "none";
  }
}

// Calculate ISO week number
function getISOWeekNumber(date) {
  const d = new Date(
    Date.UTC(date.getFullYear(), date.getMonth(), date.getDate()),
  );
  const dayNum = d.getUTCDay() || 7;
  d.setUTCDate(d.getUTCDate() + 4 - dayNum);
  const yearStart = new Date(Date.UTC(d.getUTCFullYear(), 0, 1));
  return Math.ceil(((d - yearStart) / 86400000 + 1) / 7);
}

// Calendar rendering
async function renderCalendar() {
  const calendarWidget = document.getElementById("calendarWidget");

  const monthNames = [
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
  ];

  const dayNames = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];

  // Get month logo
  const monthLogo = monthLogos[currentMonth] || monthLogos[1];

  let html = `
        <div class="calendar-header">
            <button class="calendar-nav-btn" onclick="changeMonth(-1)">‹</button>
            <div class="calendar-month-year">
                <span class="month-logo" title="${monthNames[currentMonth - 1]}">${monthLogo}</span>
                <select id="monthSelector" class="month-year-selector" onchange="changeMonthYear()">
    `;

  for (let i = 0; i < 12; i++) {
    const selected = i + 1 === currentMonth ? "selected" : "";
    html += `<option value="${i + 1}" ${selected}>${monthNames[i]}</option>`;
  }

  html += `
                </select>
                <span id="yearDisplay" class="year-display" ondblclick="editYear()" onclick="editYear()">${currentYear}</span>
                <input type="number" id="yearInput" class="year-input" value="${currentYear}"
                       onkeypress="handleYearKeyPress(event)" onblur="confirmYear()" style="display: none;">
            </div>
            <button class="calendar-nav-btn" onclick="changeMonth(1)">›</button>
        </div>
        <div class="calendar-grid">
    `;

  // Week number header
  html += '<div class="calendar-week-header">Wk</div>';

  // Day headers
  dayNames.forEach((day) => {
    html += `<div class="calendar-day-header">${day}</div>`;
  });

  // Get first day of month and number of days
  const firstDay = new Date(currentYear, currentMonth - 1, 1);
  const lastDay = new Date(currentYear, currentMonth, 0);
  const daysInMonth = lastDay.getDate();
  const startingDayOfWeek = firstDay.getDay();

  // Get events for the month
  const eventDates = await getEventDatesForMonth();
  const holidayDates = await getHolidayDatesForMonth();

  // Build calendar by weeks (rows)
  const totalDays = daysInMonth + startingDayOfWeek;
  const totalWeeks = Math.ceil(totalDays / 7);

  // Calculate days from previous month for first week
  const prevMonthLastDay = new Date(currentYear, currentMonth - 1, 0).getDate();
  let dayCounter = 1;

  for (let week = 0; week < totalWeeks; week++) {
    // Calculate week number for this week
    let weekStartDate;
    if (week === 0) {
      // First week - may start in previous month
      const firstDayOfWeek = 1 - startingDayOfWeek;
      weekStartDate = new Date(currentYear, currentMonth - 1, firstDayOfWeek);
    } else {
      // Calculate first day of this week (Sunday)
      const daysFromStart = week * 7 - startingDayOfWeek;
      weekStartDate = new Date(
        currentYear,
        currentMonth - 1,
        1 + daysFromStart,
      );
    }
    const weekNumber = getISOWeekNumber(weekStartDate);

    // Week number cell
    html += `<div class="calendar-week-number">${weekNumber}</div>`;

    // 7 days for this week
    for (let dayOfWeek = 0; dayOfWeek < 7; dayOfWeek++) {
      let date, day, isCurrentMonth;

      if (week === 0 && dayOfWeek < startingDayOfWeek) {
        // Days from previous month
        day = prevMonthLastDay - (startingDayOfWeek - dayOfWeek - 1);
        date = new Date(currentYear, currentMonth - 2, day);
        isCurrentMonth = false;
      } else if (dayCounter > daysInMonth) {
        // Days from next month
        day = dayCounter - daysInMonth;
        date = new Date(currentYear, currentMonth, day);
        isCurrentMonth = false;
      } else {
        // Current month
        day = dayCounter;
        date = new Date(currentYear, currentMonth - 1, day);
        isCurrentMonth = true;
        dayCounter++;
      }

      if (isCurrentMonth) {
        const dateStr = formatDate(date);
        const isToday = isSameDay(date, new Date());
        const isSelected = isSameDay(date, currentSelectedDate);
        const isWeekend = date.getDay() === 0 || date.getDay() === 6;

        let dayClass = "calendar-day";
        if (isToday) dayClass += " today";
        if (isSelected) dayClass += " selected";
        if (isWeekend) dayClass += " weekend";

        html += `<div class="${dayClass}" onclick="selectDate(${day})" 
                      onmouseenter="showDateTooltip('${dateStr}', this)" 
                      onmouseleave="hideDateTooltip()">`;
        html += `<div class="calendar-day-number">${day}</div>`;
        html += '<div class="calendar-day-events">';

        // Add events for this date
        const dayEvents = eventDates[dateStr] || [];
        const dayHolidays = holidayDates[dateStr] || [];

        const totalEvents = dayEvents.length + dayHolidays.length;
        
        // Show max 2 events, then "+X more"
        [...dayEvents, ...dayHolidays].slice(0, 2).forEach((event) => {
          const color = event.color || "#333";
          html += `<div class="calendar-event" style="color: ${color}">${event.text}</div>`;
        });

        if (totalEvents > 2) {
          html += `<div class="calendar-event" style="font-weight:bold;text-align:center">+${totalEvents - 2} more</div>`;
        }

        html += "</div></div>";
      } else {
        // Days from other months - empty cells
        html += '<div class="calendar-day other-month"></div>';
      }
    }
  }

  html += "</div>";
  calendarWidget.innerHTML = html;
}

async function getEventDatesForMonth() {
  const response = await fetch(
    `${API_BASE}/events/month?username=${currentUsername}&year=${currentYear}&month=${currentMonth}`,
  );
  const data = await response.json();

  const eventDates = {};
  for (const dateStr of data.dates || []) {
    const events = await fetch(
      `${API_BASE}/events?username=${currentUsername}&date=${dateStr}`,
    ).then((r) => r.json());
    eventDates[dateStr] = (events.events || []).map((event) => ({
      text:
        (event.startTime ? formatTime(event.startTime) + " " : "") +
        event.description,
      color: event.color || "#333",
    }));
  }

  return eventDates;
}

async function getHolidayDatesForMonth() {
  // Always get General holidays
  const generalResponse = await fetch(
    `${API_BASE}/holidays/month?religion=General&year=${currentYear}&month=${currentMonth}`,
  );
  const generalData = await generalResponse.json();

  const holidayDates = {};
  
  // Add General holidays
  for (const dateStr of generalData.dates || []) {
    const holidays = await fetch(
      `${API_BASE}/holidays?date=${dateStr}&religion=General`,
    ).then((r) => r.json());
    holidayDates[dateStr] = (holidays.holidays || []).map((holiday) => ({
      text: holiday,
      color: "#006400",
    }));
  }

  // Add religion-specific holidays if personal calendar and religion selected
  if (currentUserRole === "personal") {
    const religion = document.getElementById("religionComboBox").value;
    if (religion !== "Select Religion" && religion !== "General") {
      const response = await fetch(
        `${API_BASE}/holidays/month?religion=${religion}&year=${currentYear}&month=${currentMonth}`,
      );
      const data = await response.json();

      for (const dateStr of data.dates || []) {
        const holidays = await fetch(
          `${API_BASE}/holidays?date=${dateStr}&religion=${religion}`,
        ).then((r) => r.json());
        
        // Merge with existing holidays for this date
        if (!holidayDates[dateStr]) {
          holidayDates[dateStr] = [];
        }
        holidayDates[dateStr].push(...(holidays.holidays || []).map((holiday) => ({
          text: holiday,
          color: "#006400",
        })));
      }
    }
  }

  return holidayDates;
}

function changeMonth(delta) {
  currentMonth += delta;
  if (currentMonth > 12) {
    currentMonth = 1;
    currentYear++;
  } else if (currentMonth < 1) {
    currentMonth = 12;
    currentYear--;
  }

  // Enforce bounds
  if (currentYear < 1800) {
    currentYear = 1800;
    currentMonth = 1;
    alert("Our records start at 1800. Jumping back to 1800.");
  } else if (currentYear > 3000) {
    currentYear = 3000;
    currentMonth = 12;
    alert("Calendar is limited to year 3000. Jumping to 3000.");
  }

  // Update selectors
  const monthSelector = document.getElementById("monthSelector");
  const yearDisplay = document.getElementById("yearDisplay");
  if (monthSelector) monthSelector.value = currentMonth;
  if (yearDisplay) yearDisplay.textContent = currentYear;

  renderCalendar();
  updateQuote();
}

function changeMonthYear() {
  const monthSelector = document.getElementById("monthSelector");

  if (monthSelector) {
    currentMonth = parseInt(monthSelector.value);

    // Reset to first day of month
    currentSelectedDate = new Date(currentYear, currentMonth - 1, 1);

    updateEventList();
    renderCalendar();
    updateQuote();
  }
}

function editYear() {
  const yearDisplay = document.getElementById("yearDisplay");
  const yearInput = document.getElementById("yearInput");

  if (yearDisplay && yearInput) {
    yearDisplay.style.display = "none";
    yearInput.style.display = "inline-block";
    yearInput.value = currentYear;
    yearInput.focus();
    yearInput.select();
  }
}

function handleYearKeyPress(event) {
  if (event.key === "Enter") {
    event.preventDefault();
    confirmYear();
  } else if (event.key === "Escape") {
    event.preventDefault();
    cancelYearEdit();
  }
}

function confirmYear() {
  const yearDisplay = document.getElementById("yearDisplay");
  const yearInput = document.getElementById("yearInput");

  if (!yearDisplay || !yearInput) return;

  let newYear = parseInt(yearInput.value);

  // Validate and enforce bounds
  if (isNaN(newYear)) {
    newYear = currentYear;
  } else if (newYear < 1800) {
    showAppMessage(
      "Are you a time traveler? We only chart years from 1800 onward. Taking you to 1800.",
    );
    newYear = 1800;
  } else if (newYear > 3000) {
    showAppMessage(
      "Whoa, future explorer! Our calendar stops at 3000. Zooming to 3000.",
    );
    newYear = 3000;
  }

  currentYear = newYear;
  yearInput.value = currentYear;
  yearDisplay.textContent = currentYear;

  yearInput.style.display = "none";
  yearDisplay.style.display = "inline-block";

  // Reset to first day of month
  currentSelectedDate = new Date(currentYear, currentMonth - 1, 1);

  updateEventList();
  renderCalendar();
  updateQuote();
}

function cancelYearEdit() {
  const yearDisplay = document.getElementById("yearDisplay");
  const yearInput = document.getElementById("yearInput");

  if (yearDisplay && yearInput) {
    yearInput.style.display = "none";
    yearDisplay.style.display = "inline-block";
  }
}

function selectDate(day) {
  currentSelectedDate = new Date(currentYear, currentMonth - 1, day);
  updateEventList();
  renderCalendar();
}

// Show tooltip on calendar date hover
let tooltipTimeout;
let currentTooltipDate = null;

async function showDateTooltip(dateStr, element) {
  // Clear any pending tooltip
  clearTimeout(tooltipTimeout);
  
  // Remove existing tooltip immediately
  hideDateTooltip();
  
  // Don't show same tooltip again
  if (currentTooltipDate === dateStr) return;
  currentTooltipDate = dateStr;
  
  // Small delay to prevent tooltip spam
  tooltipTimeout = setTimeout(async () => {
    const response = await fetch(`${API_BASE}/events?username=${currentUsername}&date=${dateStr}`);
    const data = await response.json();
    
    // Always get General holidays
    const generalHolidayResponse = await fetch(`${API_BASE}/holidays?date=${dateStr}&religion=General`);
    const generalHolidayData = await generalHolidayResponse.json();
    let holidays = generalHolidayData.holidays || [];
    
    // Get additional holidays if personal and religion is selected
    if (currentUserRole === "personal") {
      const religion = document.getElementById("religionComboBox").value;
      if (religion !== "Select Religion" && religion !== "General") {
        const holidayResponse = await fetch(`${API_BASE}/holidays?date=${dateStr}&religion=${religion}`);
        const holidayData = await holidayResponse.json();
        holidays = [...holidays, ...(holidayData.holidays || [])];
      }
    }
    
    const events = data.events || [];
    const allItems = [...events.map(e => ({
      text: (e.startTime ? formatTime(e.startTime) + " " : "") + e.description,
      color: e.color || "#333",
      type: e.isPersonal ? "P" : "A"
    })), ...holidays.map(h => ({ text: h, color: "#006400", type: "H" }))];
    
    if (allItems.length === 0) return;
    
    // Create tooltip
    const tooltip = document.createElement("div");
    tooltip.className = "calendar-day-tooltip show";
    tooltip.id = "dateTooltip";
    
    const date = new Date(dateStr);
    const header = document.createElement("div");
    header.className = "calendar-day-tooltip-header";
    header.textContent = date.toLocaleDateString("en-US", { month: "long", day: "numeric", year: "numeric" });
    tooltip.appendChild(header);
    
    allItems.forEach(item => {
      const eventDiv = document.createElement("div");
      eventDiv.className = "calendar-day-tooltip-event";
      eventDiv.style.borderLeftColor = item.color;
      eventDiv.textContent = `[${item.type}] ${item.text}`;
      tooltip.appendChild(eventDiv);
    });
    
    document.body.appendChild(tooltip);
    
    // Position tooltip
    const rect = element.getBoundingClientRect();
    tooltip.style.position = "fixed";
    tooltip.style.left = (rect.left + rect.width / 2 - tooltip.offsetWidth / 2) + "px";
    tooltip.style.top = (rect.bottom + 10) + "px";
    
    // Adjust if off-screen
    if (tooltip.getBoundingClientRect().right > window.innerWidth) {
      tooltip.style.left = (window.innerWidth - tooltip.offsetWidth - 20) + "px";
    }
    if (tooltip.getBoundingClientRect().left < 0) {
      tooltip.style.left = "20px";
    }
  }, 200); // 200ms delay before showing tooltip
}

function hideDateTooltip() {
  clearTimeout(tooltipTimeout);
  currentTooltipDate = null;
  const tooltip = document.getElementById("dateTooltip");
  if (tooltip) tooltip.remove();
}

// Event listeners moved to DOMContentLoaded to prevent duplicates

// Context menu for events (DEPRECATED - using Edit/Delete buttons now)
function showEventContextMenu(e, event) {
  if (event.id === -1) return; // Holiday

  const menu = document.createElement("div");
  menu.style.position = "fixed";
  menu.style.left = e.pageX + "px";
  menu.style.top = e.pageY + "px";
  menu.style.background = "white";
  menu.style.border = "1px solid #ccc";
  menu.style.borderRadius = "5px";
  menu.style.padding = "5px";
  menu.style.zIndex = "1000";
  menu.style.boxShadow = "0 2px 5px rgba(0,0,0,0.2)";

  const editBtn = document.createElement("button");
  editBtn.textContent = "Edit Event";
  editBtn.style.display = "block";
  editBtn.style.width = "100%";
  editBtn.style.padding = "5px";
  editBtn.style.margin = "2px 0";
  editBtn.style.cursor = "pointer";
  editBtn.onclick = () => {
    editEvent(event);
    document.body.removeChild(menu);
  };

  const deleteBtn = document.createElement("button");
  deleteBtn.textContent = "Delete Event";
  deleteBtn.style.display = "block";
  deleteBtn.style.width = "100%";
  deleteBtn.style.padding = "5px";
  deleteBtn.style.margin = "2px 0";
  deleteBtn.style.cursor = "pointer";
  deleteBtn.onclick = () => {
    deleteEvent(event.id);
    document.body.removeChild(menu);
  };

  menu.appendChild(editBtn);
  menu.appendChild(deleteBtn);
  document.body.appendChild(menu);

  setTimeout(() => {
    document.addEventListener(
      "click",
      () => {
        if (document.body.contains(menu)) {
          document.body.removeChild(menu);
        }
      },
      { once: true },
    );
  }, 100);
}

function editEvent(event) {
  document.getElementById("eventLineEdit").value = event.description;

  if (event.startTime) {
    document.getElementById("startTimeEdit").value = convertTo24Hour(
      event.startTime,
    );
  } else {
    document.getElementById("startTimeEdit").value = "09:00";
  }

  if (event.endTime) {
    document.getElementById("endTimeEdit").value = convertTo24Hour(
      event.endTime,
    );
  } else {
    document.getElementById("endTimeEdit").value = "10:00";
  }

  const recurrenceMap = { none: 0, daily: 1, weekly: 2, monthly: 3, yearly: 4 };
  document.getElementById("recurrenceComboBox").selectedIndex =
    recurrenceMap[event.recurrence] || 0;
  
  // Update recurrence count UI when editing
  if (typeof window.updateRecurrenceCountUI === 'function') {
    window.updateRecurrenceCountUI();
  }

  const colorMap = {
    "#FF0000": 1,
    "#008000": 2,
    "#0000FF": 3,
    "#FFFF00": 4,
    "#FFA500": 5,
    "#800080": 6,
  };
  document.getElementById("colorComboBox").selectedIndex =
    colorMap[event.color] || 0;

  currentlyEditingEventId = event.id;
  document.getElementById("addEventButton").textContent = "Update Event";
}

async function deleteEvent(eventId) {
  showConfirmDialog(
    "Are you sure you want to delete this event?",
    async () => {
      const response = await fetch(`${API_BASE}/events/${eventId}`, {
        method: "DELETE",
      });

      const data = await response.json();
      if (data.success) {
        if (currentlyEditingEventId === eventId) {
          currentlyEditingEventId = -1;
          document.getElementById("addEventButton").textContent = "Add Event";
          document.getElementById("eventLineEdit").value = "";
        }
        updateEventList();
        renderCalendar();
        updateSearchSuggestions();
      } else {
        showAppMessage("Could not delete event.");
      }
    }
  );
}

// Quote update
async function updateQuote() {
  const religion =
    currentUserRole === "personal"
      ? document.getElementById("religionComboBox").value
      : "";
  const response = await fetch(
    `${API_BASE}/quote?month=${currentMonth}&role=${currentUserRole}&religion=${encodeURIComponent(religion)}`,
  );
  const data = await response.json();
  document.getElementById("quoteLabel").textContent =
    data.quote || "May your month be filled with joy and purpose.";
}

// Search suggestions
async function updateSearchSuggestions() {
  const religion =
    currentUserRole === "personal"
      ? document.getElementById("religionComboBox").value
      : "";
  const response = await fetch(
    `${API_BASE}/search/suggestions?username=${currentUsername}&role=${currentUserRole}&religion=${encodeURIComponent(religion)}`,
  );
  const data = await response.json();

  const datalist = document.getElementById("searchSuggestions");
  datalist.innerHTML = "";

  (data.suggestions || []).forEach((suggestion) => {
    const option = document.createElement("option");
    option.value = suggestion;
    datalist.appendChild(option);
  });
}

// Utility functions
function formatDate(date) {
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, "0");
  const day = String(date.getDate()).padStart(2, "0");
  return `${year}-${month}-${day}`;
}

function formatTime(timeStr) {
  // Convert HH:mm to h:mm AP format
  const [hours, minutes] = timeStr.split(":");
  const hour = parseInt(hours);
  const ampm = hour >= 12 ? "PM" : "AM";
  const hour12 = hour % 12 || 12;
  return `${hour12}:${minutes} ${ampm}`;
}

function convertTo24Hour(timeStr) {
  // If already in 24-hour format (HH:mm), return as is
  if (/^\d{2}:\d{2}$/.test(timeStr)) {
    return timeStr;
  }

  // Convert h:mm AP to HH:mm
  const match = timeStr.match(/(\d+):(\d+)\s*(AM|PM)/i);
  if (!match) return timeStr || "09:00";

  let hour = parseInt(match[1]);
  const minute = match[2];
  const ampm = match[3].toUpperCase();

  if (ampm === "PM" && hour !== 12) hour += 12;
  if (ampm === "AM" && hour === 12) hour = 0;

  return `${String(hour).padStart(2, "0")}:${minute}`;
}

function isSameDay(date1, date2) {
  return (
    date1.getFullYear() === date2.getFullYear() &&
    date1.getMonth() === date2.getMonth() &&
    date1.getDate() === date2.getDate()
  );
}

// Initialize theme and all event listeners
document.addEventListener("DOMContentLoaded", () => {
  setTheme(currentTheme);

  // Populate university and department datalists from C++ backend
  async function loadUniversitiesAndDepartments() {
    try {
      const uniResponse = await fetch(`${API_BASE}/universities`);
      const uniData = await uniResponse.json();
      const universityList = document.getElementById('universityList');
      if (universityList && uniData.universities) {
        uniData.universities.forEach(uni => {
          const option = document.createElement('option');
          option.value = uni;
          universityList.appendChild(option);
        });
      }

      const deptResponse = await fetch(`${API_BASE}/departments`);
      const deptData = await deptResponse.json();
      const departmentList = document.getElementById('departmentList');
      if (departmentList && deptData.departments) {
        window.DEPARTMENTS = deptData.departments;
        Object.entries(deptData.departments).forEach(([code, name]) => {
          const option = document.createElement('option');
          option.value = `${code} - ${name}`;
          option.setAttribute('data-code', code);
          departmentList.appendChild(option);
        });
      }
    } catch (error) {
      console.error('Failed to load universities/departments:', error);
    }
  }
  
  loadUniversitiesAndDepartments();

  // Student roll number input - show department when entered
  const studentRollInput = document.getElementById('studentRollLineEdit');
  const deptDisplay = document.getElementById('studentDepartmentDisplay');
  const deptCodeEl = document.getElementById('deptCode');
  const deptNameEl = document.getElementById('deptName');
  
  if (studentRollInput && deptDisplay && deptCodeEl && deptNameEl) {
    studentRollInput.addEventListener('input', async () => {
      const roll = studentRollInput.value.trim();
      if (roll && roll.length >= 3) {
        try {
          const response = await fetch(`${API_BASE}/validate/roll?roll=${encodeURIComponent(roll)}`);
          const data = await response.json();
          
          if (data.valid && data.department && data.departmentName) {
            deptCodeEl.textContent = data.department;
            deptNameEl.textContent = data.departmentName;
            deptDisplay.classList.remove('error');
            deptDisplay.style.display = 'flex';
          } else if (data.department) {
            deptCodeEl.textContent = `⚠️ ${data.department}`;
            deptNameEl.textContent = 'Unknown Department Code';
            deptDisplay.classList.add('error');
            deptDisplay.style.display = 'flex';
          } else {
            deptDisplay.style.display = 'none';
          }
        } catch (error) {
          deptDisplay.style.display = 'none';
        }
      } else {
        deptDisplay.style.display = 'none';
      }
    });
  }

  // Add theme switcher event listeners
  document.querySelectorAll(".theme-btn").forEach((btn) => {
    btn.addEventListener("click", () => {
      setTheme(btn.dataset.theme);
    });
  });

  // ESC key handler - Close modals if open, otherwise logout
  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") {
      // Check if any modal is open
      const taskModalOpen = document.getElementById('taskModalOverlay')?.classList.contains('active');
      const appModalOpen = document.getElementById('appModalOverlay')?.style.display === 'flex';
      const shortcutsModalOpen = document.getElementById('shortcutsModalOverlay')?.classList.contains('active');
      const deptEventModalOpen = document.getElementById('teacherDeptEventModalOverlay')?.classList.contains('active');
      const availabilityModalOpen = document.getElementById('teacherAvailabilityModalOverlay')?.classList.contains('active');
      const findTeacherModalOpen = document.getElementById('studentFindTeacherModalOverlay')?.classList.contains('active');
      
      if (taskModalOpen || appModalOpen || shortcutsModalOpen || deptEventModalOpen || availabilityModalOpen || findTeacherModalOpen) {
        // Close modals
        closeAppModal();
        closeTaskManager();
        closeShortcutsModal();
        if (typeof closeDeptEventModal !== 'undefined') closeDeptEventModal();
        if (typeof closeAvailabilityModal !== 'undefined') closeAvailabilityModal();
        if (typeof closeFindTeacherModal !== 'undefined') closeFindTeacherModal();
      } else if (document.getElementById('calendarPage')?.classList.contains('active')) {
        // No modals open and on calendar page - show logout confirmation
        showLogoutConfirmation();
      }
    }
  });

  // Notepad Button
  document.getElementById('openTaskManagerButton')?.addEventListener('click', openTaskManager);

  // Notepad textarea events
  const notepadTextarea = document.getElementById('notepadTextarea');
  if (notepadTextarea) {
    notepadTextarea.addEventListener('input', () => {
      autoSaveNotepad();
      updateNotepadStats();
    });
    
    // Auto-continue bullet points on Enter
    notepadTextarea.addEventListener('keydown', (e) => {
      if (e.key === 'Enter') {
        const textarea = e.target;
        const cursorPos = textarea.selectionStart;
        const textBeforeCursor = textarea.value.substring(0, cursorPos);
        const currentLine = textBeforeCursor.split('\n').pop();
        
        // Check if current line starts with bullet, star, arrow, dash, or heading
        const bulletMatch = currentLine.match(/^(\s*)(•|★|→|-|#)\s/);
        
        if (bulletMatch) {
          e.preventDefault();
          const indent = bulletMatch[1];
          const bullet = bulletMatch[2];
          const textAfterCursor = textarea.value.substring(cursorPos);
          
          // Insert newline with same bullet
          const newText = '\n' + indent + bullet + ' ';
          textarea.value = textBeforeCursor + newText + textAfterCursor;
          textarea.selectionStart = textarea.selectionEnd = cursorPos + newText.length;
          
          autoSaveNotepad();
          updateNotepadStats();
        }
      }
    });
  }

  // Close modal on overlay click
  document.getElementById('taskModalOverlay')?.addEventListener('click', (e) => {
    if (e.target.id === 'taskModalOverlay') closeTaskManager();
  });

  // Tab switching
  document.querySelectorAll(".tab-btn").forEach((btn) => {
    btn.addEventListener("click", () => {
      const tab = btn.dataset.tab;
      document
        .querySelectorAll(".tab-btn")
        .forEach((b) => b.classList.remove("active"));
      document
        .querySelectorAll(".tab-content")
        .forEach((c) => c.classList.remove("active"));
      btn.classList.add("active");
      document.getElementById(tab + "Tab").classList.add("active");
    });
  });

  // Login handlers
  document
    .getElementById("personalCalendarButton")
    .addEventListener("click", () => {
      showPage("personalLoginPage");
    });
  document
    .getElementById("backFromPersonalButton")
    .addEventListener("click", () => {
      showPage("loginPage");
    });
  document
    .getElementById("personalLoginButton")
    .addEventListener("click", async () => {
      const username = document.getElementById("personalUsername").value.trim();
      const password = document.getElementById("personalPassword").value;
      if (!username || !password)
        return showAppMessage("Please enter username and password.");
      const res = await fetch(`${API_BASE}/user/validate`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, password, role: "personal" }),
      });
      const data = await res.json();
      if (!data.valid) return showAppMessage("Invalid username or password.");
      currentUsername = username;
      currentUserRole = "personal";
      setupCalendarPage();
    });
  document
    .getElementById("personalRegisterButton")
    .addEventListener("click", async () => {
      const username = document.getElementById("personalUsername").value.trim();
      const password = document.getElementById("personalPassword").value;
      if (!username || !password)
        return showAppMessage("Create a username and password first.");
      const res = await fetch(`${API_BASE}/user/add`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, password, role: "personal" }),
      });
      const data = await res.json();
      if (!data.success) {
        if (data.exists)
          return showAppMessage(
            "This username already exists. Try logging in.",
          );
        return showAppMessage("Registration failed. Please try again.");
      }
      showAppMessage("Registered successfully. You can now login.");
    });

  document
    .getElementById("academicCalendarButton")
    .addEventListener("click", () => {
      showPage("academicLoginPage");
    });

  // Admin entry
  document
    .getElementById("adminCalendarButton")
    .addEventListener("click", () => {
      showPage("adminLoginPage");
    });

  // Admin login
  document
    .getElementById("adminLoginButton")
    .addEventListener("click", async () => {
      const username = document.getElementById("adminUsername").value.trim();
      const password = document.getElementById("adminPassword").value;
      const response = await fetch(`${API_BASE}/user/validate`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, password, role: "admin" }),
      });
      const data = await response.json();
      if (data.valid) {
        currentUsername = "admin";
        currentUserRole = "admin";
        loadAdminUsers();
        showPage("adminPage");
      } else {
        alert("Invalid admin credentials.");
      }
    });
  document
    .getElementById("backFromAdminButton")
    .addEventListener("click", () => {
      showPage("loginPage");
    });
  document.getElementById("adminLogoutButton").addEventListener("click", () => {
    currentUsername = "";
    currentUserRole = "";
    showPage("loginPage");
  });

  document
    .getElementById("adminTogglePassword")
    .addEventListener("click", () => {
      const input = document.getElementById("adminNewPassword");
      if (input.type === "password") {
        input.type = "text";
        document.getElementById("adminTogglePassword").textContent = "Hide";
      } else {
        input.type = "password";
        document.getElementById("adminTogglePassword").textContent = "Show";
      }
    });

  document
    .getElementById("adminChangePasswordButton")
    .addEventListener("click", async () => {
      const username = document
        .getElementById("adminTargetUsername")
        .value.trim();
      const role = document.getElementById("adminTargetRole").value.trim();
      const password = document.getElementById("adminNewPassword").value;
      if (!username || !role || !password) {
        alert("Please fill all fields.");
        return;
      }
      const res = await fetch(`${API_BASE}/users/password`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ admin: "true", username, role, password }),
      });
      const data = await res.json();
      if (data.success) {
        showAppMessage("Password updated.");
      } else {
        showAppMessage(
          "No such user found for given role. Please check username/role.",
        );
      }
    });

  document
    .getElementById("adminDeleteUserButton")
    .addEventListener("click", async () => {
      const username = document
        .getElementById("adminDeleteUsername")
        .value.trim();
      const role = document.getElementById("adminDeleteRole").value.trim();
      if (!username || !role) {
        alert("Provide username and role.");
        return;
      }
      const res = await fetch(
        `${API_BASE}/users?admin=true&username=${encodeURIComponent(username)}&role=${encodeURIComponent(role)}`,
        {
          method: "DELETE",
        },
      );
      const data = await res.json();
      if (data.success) {
        alert("User deleted.");
        loadAdminUsers();
      } else {
        alert("Failed to delete user.");
      }
    });

  document
    .getElementById("studentLoginButton")
    .addEventListener("click", async () => {
      const name = document.getElementById("studentNameLineEdit").value.trim();
      const roll = document.getElementById("studentRollLineEdit").value.trim();
      const university = document.getElementById("studentUniversityLineEdit").value.trim();
      const password = document.getElementById("studentPasswordLineEdit").value;

      if (!name || !roll) {
        alert("Name and Roll Number cannot be empty.");
        return;
      }

      // Use getDepartmentFromRoll from universities.js
      const department = typeof getDepartmentFromRoll !== 'undefined' ? getDepartmentFromRoll(roll) : roll.match(/^([A-Z]{2,3})-/) ? roll.match(/^([A-Z]{2,3})-/)[1] : "";
      
      if (!department) {
        alert('Invalid roll number format. Must start with department code (e.g., CT-123, SE-456).');
        return;
      }

      // Validate department exists in DEPARTMENTS
      if (typeof DEPARTMENTS !== 'undefined' && !DEPARTMENTS[department]) {
        alert(`Unknown department code: ${department}. Please use a valid department code.`);
        return;
      }

      // Validate login with university
      const response = await fetch(`${API_BASE}/user/validate`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username: roll, password, role: "student", university }),
      });
      const data = await response.json();
      if (data.valid) {
        currentUsername = roll;
        currentUserRole = "student";
        currentUserDisplayName = data.displayName || name;
        currentUserUniversity = data.university || university;
        currentUserDepartment = data.department || department;
        setupCalendarPage();
      } else {
        const errorMsg = data.error || "Invalid credentials";
        alert(errorMsg);
      }
    });

  document
    .getElementById("studentRegisterButton")
    .addEventListener("click", async () => {
      const name = document.getElementById("studentNameLineEdit").value.trim();
      const roll = document.getElementById("studentRollLineEdit").value.trim();
      const university = document.getElementById("studentUniversityLineEdit").value.trim();
      const password = document.getElementById("studentPasswordLineEdit").value;
      if (!name || !roll || !password) {
        alert("Fill name, roll and password.");
        return;
      }
      // Use getDepartmentFromRoll from universities.js
      const department = typeof getDepartmentFromRoll !== 'undefined' ? getDepartmentFromRoll(roll) : roll.match(/^([A-Z]{2,3})-/) ? roll.match(/^([A-Z]{2,3})-/)[1] : "";
      
      if (!department) {
        alert('Invalid roll number format. Must start with department code (e.g., CT-123, SE-456).');
        return;
      }

      // Validate department exists in DEPARTMENTS
      if (typeof DEPARTMENTS !== 'undefined' && !DEPARTMENTS[department]) {
        alert(`Unknown department code: ${department}. Please use a valid department code.`);
        return;
      }

      const res = await fetch(`${API_BASE}/user/add`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username: roll, password, role: "student", displayName: name, university, department }),
      });
      const data = await res.json();
      if (!data.success) {
        if (data.exists) {
          alert("This roll already exists.");
        } else {
          alert("Registration failed.");
        }
        return;
      }
      alert("Registered. Please login.");
    });
  document
    .getElementById("teacherLoginButton")
    .addEventListener("click", async () => {
      const username = document
        .getElementById("teacherUsernameLineEdit")
        .value.trim();
      const university = document.getElementById("teacherUniversityLineEdit").value.trim();
      const departmentInput = document.getElementById("teacherDepartmentLineEdit").value.trim();
      // Extract department code from "CT - Computer Science" format
      const department = departmentInput.match(/^([A-Z]{2,3})\s*-/) ? departmentInput.match(/^([A-Z]{2,3})\s*-/)[1] : departmentInput;
      const password = document.getElementById("teacherPasswordLineEdit").value;

      const response = await fetch(`${API_BASE}/user/validate`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, password, role: "teacher", university, department }),
      });

      const data = await response.json();
      if (data.valid) {
        currentUsername = username;
        currentUserRole = "teacher";
        currentUserDisplayName = data.displayName || username;
        currentUserUniversity = data.university || university;
        currentUserDepartment = data.department || department;
        setupCalendarPage();
      } else {
        const errorMsg = data.error || "Invalid credentials";
        alert(errorMsg);
      }
    });

  document
    .getElementById("teacherRegisterButton")
    .addEventListener("click", async () => {
      const username = document
        .getElementById("teacherUsernameLineEdit")
        .value.trim();
      const university = document.getElementById("teacherUniversityLineEdit").value.trim();
      const departmentInput = document.getElementById("teacherDepartmentLineEdit").value.trim();
      // Extract department code from "CT - Computer Science" format
      const department = departmentInput.match(/^([A-Z]{2,3})\s*-/) ? departmentInput.match(/^([A-Z]{2,3})\s*-/)[1] : departmentInput;
      const password = document.getElementById("teacherPasswordLineEdit").value;

      if (!username || !password) {
        alert("Username and password cannot be empty.");
        return;
      }
      if (!university || !department) {
        alert("Please select university and department.");
        return;
      }

      const response = await fetch(`${API_BASE}/user/add`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username, password, role: "teacher", displayName: username, university, department }),
      });

      const data = await response.json();
      if (data.success) {
        alert("Teacher registered successfully. Please login.");
        document.getElementById("teacherUsernameLineEdit").value = "";
        document.getElementById("teacherPasswordLineEdit").value = "";
        document.getElementById("teacherUniversityLineEdit").value = "";
        document.getElementById("teacherDepartmentLineEdit").value = "";
      } else {
        alert("Registration failed. Username may already exist.");
      }
    });

  document
    .getElementById("backToMainLoginButton")
    .addEventListener("click", () => {
      showPage("loginPage");
    });

  document.getElementById("backToLoginButton").addEventListener("click", () => {
    currentUsername = "";
    currentUserRole = "";
    currentUserDisplayName = "";
    currentUserUniversity = "";
    currentUserDepartment = "";
    currentSelectedDate = new Date();
    document.getElementById("eventListWidget").innerHTML = "";
    document.getElementById("searchLineEdit").value = "";
    showPage("loginPage");
  });

  // Recurrence count input visibility and label update
  const recurrenceComboBox = document.getElementById("recurrenceComboBox");
  const recurrenceCountGroup = document.getElementById("recurrenceCountGroup");
  const recurrenceCountLabel = document.getElementById("recurrenceCountLabel");
  
  window.updateRecurrenceCountUI = function() {
    const selectedIndex = recurrenceComboBox.selectedIndex;
    if (selectedIndex === 0) {
      // "Doesn't Repeat"
      recurrenceCountGroup.style.display = "none";
    } else {
      recurrenceCountGroup.style.display = "flex";
      const labels = ["", "days", "weeks", "months", "years"];
      const labelText = labels[selectedIndex] || "";
      recurrenceCountLabel.textContent = labelText ? `(${labelText})` : "";
    }
  };
  
  recurrenceComboBox.addEventListener("change", window.updateRecurrenceCountUI);
  window.updateRecurrenceCountUI(); // Initialize on page load

  // Add event button
  document.getElementById("addEventButton").addEventListener("click", async () => {
    const description = document.getElementById("eventLineEdit").value.trim();
    if (!description) {
      alert("Event description cannot be empty.");
      return;
    }

    const dateStr = formatDate(currentSelectedDate);
    const startTime = document.getElementById("startTimeEdit").value;
    const endTime = document.getElementById("endTimeEdit").value;
    const recurrenceIndex = document.getElementById("recurrenceComboBox").selectedIndex;
    const recurrenceCountInput = document.getElementById("recurrenceCountInput");
    let recurrenceCount = 0; // 0 = unlimited
    if (recurrenceIndex > 0) { // If recurrence is selected (not "Doesn't Repeat")
      if (recurrenceCountInput && recurrenceCountInput.value.trim() !== "") {
        const parsed = parseInt(recurrenceCountInput.value);
        if (!isNaN(parsed) && parsed >= 0) {
          recurrenceCount = parsed; // Use the entered value (0 = unlimited, >0 = that many occurrences)
        }
      }
      // If empty, keep as 0 (unlimited)
    }
    
    const colorIndex = document.getElementById("colorComboBox").selectedIndex;
    const categoryIndex = document.getElementById("categoryComboBox").selectedIndex;

    const recurrence = ["none", "daily", "weekly", "monthly", "yearly"][recurrenceIndex];
    const colorNames = ["", "Red", "Green", "Blue", "Yellow", "Orange", "Purple"];
    const color = colorNames[colorIndex];
    const categoryNames = ["", "Work", "Personal", "Study", "Health", "Social", "Travel"];
    const categoryIcons = ["", "🏢", "👨‍👩‍👧", "📚", "💪", "🎉", "✈️"];
    const category = categoryNames[categoryIndex];
    const categoryIcon = categoryIcons[categoryIndex];

    // Quick-add events are always personal for all roles
    const isPersonal = true;
    const usernameForEvent = currentUsername;

    if (currentlyEditingEventId !== -1) {
      // Update event
      const response = await fetch(`${API_BASE}/events/${currentlyEditingEventId}`, {
        method: "PUT",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          description,
          startTime: startTime || "",
          endTime: endTime || "",
          color,
        }),
      });

      const data = await response.json();
      if (!data.success) {
        alert("Could not update event.");
        return;
      }

      currentlyEditingEventId = -1;
      document.getElementById("addEventButton").textContent = "Add Event";
    } else {
      // Add event
      const response = await fetch(`${API_BASE}/events`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          username: usernameForEvent,
          date: dateStr,
          description,
          startTime: startTime || "",
          endTime: endTime || "",
          color,
          isPersonal,
          recurrence,
          recurrenceCount,
          category: category || "",
          categoryIcon: categoryIcon || "",
        }),
      });

      const data = await response.json();
      if (!data.success) {
        alert("Could not add event.");
        return;
      }
    }

    // Reset form
    document.getElementById("eventLineEdit").value = "";
    document.getElementById("recurrenceComboBox").selectedIndex = 0;
    const resetCountInput = document.getElementById("recurrenceCountInput");
    if (resetCountInput) resetCountInput.value = "";
    if (typeof window.updateRecurrenceCountUI === 'function') {
      window.updateRecurrenceCountUI();
    }
    document.getElementById("colorComboBox").selectedIndex = 0;
    document.getElementById("startTimeEdit").value = "09:00";
    document.getElementById("endTimeEdit").value = "10:00";

    updateEventList();
    renderCalendar();
    updateSearchSuggestions();
  });

  // Edit event button
  document.getElementById("editEventButton")?.addEventListener("click", () => {
    const event = window.selectedEventData;
    if (!event) return;
    
    // Students cannot edit department events (non-personal events)
    if (currentUserRole === "student" && !event.isPersonal) {
      showAppMessage("You cannot edit department events. Only teachers can modify department events.");
      return;
    }
    
    document.getElementById("eventLineEdit").value = event.description;
    document.getElementById("startTimeEdit").value = event.startTime || "09:00";
    document.getElementById("endTimeEdit").value = event.endTime || "10:00";
    
    const recurrenceMap = { none: 0, daily: 1, weekly: 2, monthly: 3, yearly: 4 };
    document.getElementById("recurrenceComboBox").selectedIndex = recurrenceMap[event.recurrence] || 0;
    
    const colorMap = { "#FF0000": 1, "#008000": 2, "#0000FF": 3, "#FFFF00": 4, "#FFA500": 5, "#800080": 6 };
    document.getElementById("colorComboBox").selectedIndex = colorMap[event.color] || 0;
    
    currentlyEditingEventId = event.id;
    document.getElementById("addEventButton").textContent = "Update Event";
    document.getElementById("eventActions").style.display = "none";
  });

  // Delete event button
  document.getElementById("deleteEventButton")?.addEventListener("click", () => {
    const event = window.selectedEventData;
    if (!event) return;
    
    // Students cannot delete department events (non-personal events)
    if (currentUserRole === "student" && !event.isPersonal) {
      showAppMessage("You cannot delete department events. Only teachers can delete department events.");
      return;
    }
    
    // Show custom confirmation modal
    showDeleteConfirmation(event);
  });

  document.getElementById("goToTodayButton").addEventListener("click", () => {
    currentSelectedDate = new Date();
    currentMonth = currentSelectedDate.getMonth() + 1;
    currentYear = currentSelectedDate.getFullYear();

    // Update selectors
    const monthSelector = document.getElementById("monthSelector");
    const yearDisplay = document.getElementById("yearDisplay");
    if (monthSelector) monthSelector.value = currentMonth;
    if (yearDisplay) yearDisplay.textContent = currentYear;

    updateEventList();
    updateQuote();
    renderCalendar();
  });

  // Search
  document
    .getElementById("searchButton")
    .addEventListener("click", async () => {
      const searchTerm = document.getElementById("searchLineEdit").value.trim();

      if (!searchTerm) {
        // Show all events dropdown
        const response = await fetch(
          `${API_BASE}/events/all?username=${currentUsername}`,
        );
        const data = await response.json();

        if (!data.events || data.events.length === 0) {
          alert("You have no events in your calendar.");
          return;
        }

        // Create dropdown menu
        const menu = document.createElement("div");
        menu.id = "allEventsMenu";
        menu.style.position = "fixed";
        menu.style.background = "white";
        menu.style.border = "1px solid #ccc";
        menu.style.borderRadius = "5px";
        menu.style.padding = "5px";
        menu.style.maxHeight = "300px";
        menu.style.overflowY = "auto";
        menu.style.zIndex = "1000";
        menu.style.boxShadow = "0 2px 10px rgba(0,0,0,0.2)";

        const searchBtn = document.getElementById("searchButton");
        const rect = searchBtn.getBoundingClientRect();
        menu.style.left = rect.left + "px";
        menu.style.top = rect.bottom + 5 + "px";
        menu.style.minWidth = "250px";

        data.events.forEach((event) => {
          const item = document.createElement("div");
          item.style.padding = "8px";
          item.style.cursor = "pointer";
          item.style.borderBottom = "1px solid #eee";
          item.onmouseover = () => (item.style.background = "#f0f0f0");
          item.onmouseout = () => (item.style.background = "white");

          const date = new Date(event.date);
          const dateStr = date.toLocaleDateString("en-US", {
            month: "short",
            day: "numeric",
            year: "numeric",
          });
          item.textContent = `${dateStr}: ${event.description}`;

          item.onclick = () => {
            currentSelectedDate = date;
            currentMonth = date.getMonth() + 1;
            currentYear = date.getFullYear();
            updateEventList();
            renderCalendar();
            document.body.removeChild(menu);
          };

          menu.appendChild(item);
        });

        document.body.appendChild(menu);

        // Close menu when clicking outside
        setTimeout(() => {
          document.addEventListener(
            "click",
            (e) => {
              if (!menu.contains(e.target) && e.target !== searchBtn) {
                if (document.body.contains(menu)) {
                  document.body.removeChild(menu);
                }
              }
            },
            { once: true },
          );
        }, 100);

        return;
      }

      if (searchTerm.length < 2) {
        alert("Please enter at least 2 characters to search.");
        return;
      }

      const religion =
        currentUserRole === "personal"
          ? document.getElementById("religionComboBox").value
          : "";

      // Guard: block religious holiday navigation unless matching religion is selected
      // Lightweight keyword-based detector mapped to religions
      const keywordReligionMap = [
        {
          rel: "Islam",
          keys: ["ramadan", "ramzan", "eid", "mawlid", "ashura", "hijri"],
        },
        {
          rel: "Christian",
          keys: [
            "easter",
            "christmas",
            "good friday",
            "ascension",
            "pentecost",
            "epiphany",
          ],
        },
        {
          rel: "Hindu",
          keys: [
            "diwali",
            "deepavali",
            "holi",
            "navratri",
            "janmashtami",
            "maha shivaratri",
            "ram navami",
            "raksha bandhan",
            "dussera",
            "dussehra",
          ],
        },
        {
          rel: "Jewish",
          keys: [
            "passover",
            "pesach",
            "rosh hashanah",
            "yom kippur",
            "hanukkah",
            "chanukah",
            "purim",
            "sukkot",
            "shavuot",
          ],
        },
        {
          rel: "Sikh",
          keys: ["vaisakhi", "baisakhi", "guru nanak", "gurpurab"],
        },
      ];
      const termLower = searchTerm.toLowerCase();
      let detectedReligion = "";
      for (const entry of keywordReligionMap) {
        if (entry.keys.some((k) => termLower.includes(k))) {
          detectedReligion = entry.rel;
          break;
        }
      }

      // If the term looks like a religious holiday but religion is not selected or mismatched, block and prompt
      if (currentUserRole === "personal" && detectedReligion) {
        const hasValidSelection = religion && religion !== "Select Religion";
        const matchesSelected =
          hasValidSelection && religion === detectedReligion;
        if (!matchesSelected) {
          // Prefer in-app modal if available; fallback to alert
          if (typeof openAppModal === "function") {
            const msg = hasValidSelection
              ? `This looks like a ${detectedReligion} holiday. Please switch religion to '${detectedReligion}' to search it.`
              : `This looks like a ${detectedReligion} holiday. Please select '${detectedReligion}' in the religion list to search it.`;
            openAppModal(msg);
          } else {
            const msg = hasValidSelection
              ? `This looks like a ${detectedReligion} holiday. Switch religion to '${detectedReligion}' to search it.`
              : `This looks like a ${detectedReligion} holiday. Select '${detectedReligion}' in the religion list to search it.`;
            alert(msg);
          }
          return;
        }
      }

      const religionParam =
        currentUserRole === "personal" &&
        religion &&
        religion !== "Select Religion"
          ? `&religion=${encodeURIComponent(religion)}`
          : "";
      const response = await fetch(
        `${API_BASE}/events/search?username=${currentUsername}&term=${encodeURIComponent(searchTerm)}${religionParam}`,
      );
      const data = await response.json();

      if (data.results.length === 0) {
        alert(`No events or holidays found matching '${searchTerm}'.`);
        return;
      }

      // Prefer results in the current year, then current month, then future dates
      let targetResult = null;

      // First, try to find a result in the current year
      const currentYearResults = data.results.filter((r) => {
        const resultDate = new Date(r.date);
        return resultDate.getFullYear() === currentYear;
      });

      if (currentYearResults.length > 0) {
        // Within current year, prefer current month or future
        const currentMonthResults = currentYearResults.filter((r) => {
          const resultDate = new Date(r.date);
          return resultDate.getMonth() + 1 >= currentMonth;
        });

        if (currentMonthResults.length > 0) {
          // Prefer dates on/after current selected date
          const futureResults = currentMonthResults.filter((r) => {
            const resultDate = new Date(r.date);
            return resultDate >= currentSelectedDate;
          });
          targetResult =
            futureResults.length > 0
              ? futureResults[0]
              : currentMonthResults[0];
        } else {
          // No results in current month, take first in current year
          targetResult = currentYearResults[0];
        }
      } else {
        // No results in current year, prefer future dates
        const futureResults = data.results.filter((r) => {
          const resultDate = new Date(r.date);
          return resultDate >= currentSelectedDate;
        });
        targetResult =
          futureResults.length > 0 ? futureResults[0] : data.results[0];
      }

      // Jump to the target result
      const resultDate = new Date(targetResult.date);
      currentSelectedDate = resultDate;
      currentMonth = resultDate.getMonth() + 1;
      currentYear = resultDate.getFullYear();

      // Update selectors
      const monthSelector = document.getElementById("monthSelector");
      const yearDisplay = document.getElementById("yearDisplay");
      if (monthSelector) monthSelector.value = currentMonth;
      if (yearDisplay) yearDisplay.textContent = currentYear;

      // Update event list and calendar
      updateEventList();
      renderCalendar();
    });

  // Religion change
  document.getElementById("religionComboBox").addEventListener("change", () => {
    updateEventList();
    updateQuote();
    renderCalendar();
    updateSearchSuggestions();
  });

  document
    .getElementById("addTaskButton")
    ?.addEventListener("click", async () => {
      const title = document.getElementById("taskTitleInput").value.trim();
      const due = document.getElementById("taskDueInput").value.trim();
      if (!title) {
        alert("Task title is required");
        return;
      }
      await fetch(`${API_BASE}/tasks`, {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ username: currentUsername, title, due }),
      });
      document.getElementById("taskTitleInput").value = "";
      document.getElementById("taskDueInput").value = "";
      refreshTasks();
    });
  document
    .getElementById("refreshTasksButton")
    ?.addEventListener("click", refreshTasks);

  // Initialize
  document.getElementById("startTimeEdit").value = "09:00";
  document.getElementById("endTimeEdit").value = "10:00";
  
  // Time preset buttons
  document.querySelectorAll('.time-preset-btn').forEach(btn => {
    btn.addEventListener('click', (e) => {
      e.preventDefault();
      const startTime = btn.dataset.start;
      const endTime = btn.dataset.end;
      document.getElementById('startTimeEdit').value = startTime;
      document.getElementById('endTimeEdit').value = endTime;
      
      // Visual feedback
      document.querySelectorAll('.time-preset-btn').forEach(b => b.style.background = '');
      btn.style.background = 'var(--brand)';
      btn.style.color = 'white';
      setTimeout(() => {
        btn.style.background = '';
        btn.style.color = '';
      }, 300);
    });
  });
  
  // Duration buttons
  document.querySelectorAll('.duration-btn').forEach(btn => {
    btn.addEventListener('click', (e) => {
      e.preventDefault();
      const minutes = parseInt(btn.dataset.minutes);
      const startTimeInput = document.getElementById('startTimeEdit');
      const endTimeInput = document.getElementById('endTimeEdit');
      
      // Get start time
      const startTime = startTimeInput.value;
      if (!startTime) {
        startTimeInput.value = '09:00';
      }
      
      // Calculate end time
      const [hours, mins] = startTimeInput.value.split(':').map(Number);
      const startDate = new Date();
      startDate.setHours(hours, mins, 0, 0);
      
      const endDate = new Date(startDate.getTime() + minutes * 60000);
      const endHours = String(endDate.getHours()).padStart(2, '0');
      const endMins = String(endDate.getMinutes()).padStart(2, '0');
      
      endTimeInput.value = `${endHours}:${endMins}`;
      
      // Visual feedback
      document.querySelectorAll('.duration-btn').forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      setTimeout(() => btn.classList.remove('active'), 500);
    });
  });
  
  // Auto-calculate duration when start time changes
  document.getElementById('startTimeEdit').addEventListener('change', () => {
    const startTimeInput = document.getElementById('startTimeEdit');
    const endTimeInput = document.getElementById('endTimeEdit');
    
    if (startTimeInput.value && !endTimeInput.value) {
      // Default to 1 hour if no end time
      const [hours, mins] = startTimeInput.value.split(':').map(Number);
      const startDate = new Date();
      startDate.setHours(hours, mins, 0, 0);
      const endDate = new Date(startDate.getTime() + 60 * 60000);
      const endHours = String(endDate.getHours()).padStart(2, '0');
      const endMins = String(endDate.getMinutes()).padStart(2, '0');
      endTimeInput.value = `${endHours}:${endMins}`;
    }
  });
});


// Calendar View Management
let currentView = 'month'; // 'month', 'week', 'day'

function setCalendarView(view) {
  currentView = view;
  localStorage.setItem('calendarView', view);
  
  // Update active button
  document.querySelectorAll('.view-btn').forEach(btn => {
    btn.classList.toggle('active', btn.dataset.view === view);
  });
  
  renderCalendar();
}

// Render Week View
async function renderWeekView() {
  const calendarWidget = document.getElementById("calendarWidget");
  
  // Get start of week (Sunday)
  const startOfWeek = new Date(currentSelectedDate);
  startOfWeek.setDate(currentSelectedDate.getDate() - currentSelectedDate.getDay());
  
  const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];
  
  let html = `
    <div class="calendar-header">
      <button class="calendar-nav-btn" onclick="changeWeek(-1)">‹</button>
      <div class="calendar-month-year">
        <span style="font-size:16pt">Week View</span>
      </div>
      <button class="calendar-nav-btn" onclick="changeWeek(1)">›</button>
    </div>
    <div class="week-view-grid">
  `;
  
  // Render 7 days
  for (let i = 0; i < 7; i++) {
    const date = new Date(startOfWeek);
    date.setDate(startOfWeek.getDate() + i);
    const dateStr = formatDate(date);
    const isToday = isSameDay(date, new Date());
    const isSelected = isSameDay(date, currentSelectedDate);
    
    // Fetch events for this day
    const response = await fetch(`${API_BASE}/events?username=${currentUsername}&date=${dateStr}`);
    const data = await response.json();
    const events = data.events || [];
    
    html += `
      <div class="week-day-column ${isToday ? 'today' : ''} ${isSelected ? 'selected' : ''}" onclick="selectDate(${date.getDate()}, ${date.getMonth() + 1}, ${date.getFullYear()})">
        <div class="week-day-header">
          <div class="week-day-name">${dayNames[i]}</div>
          <div class="week-day-number">${date.getDate()}</div>
        </div>
        <div class="week-day-events">
    `;
    
    events.forEach(event => {
      const color = event.color || '#333';
      html += `
        <div class="week-event" style="border-left-color: ${color}">
          <div class="week-event-time">${event.startTime ? formatTime(event.startTime) : 'All day'}</div>
          <div class="week-event-title">${event.description}</div>
        </div>
      `;
    });
    
    html += `
        </div>
      </div>
    `;
  }
  
  html += '</div>';
  calendarWidget.innerHTML = html;
}

// Render Day View
async function renderDayView() {
  const calendarWidget = document.getElementById("calendarWidget");
  const dateStr = formatDate(currentSelectedDate);
  
  const response = await fetch(`${API_BASE}/events?username=${currentUsername}&date=${dateStr}`);
  const data = await response.json();
  const events = data.events || [];
  
  const dayName = currentSelectedDate.toLocaleDateString('en-US', { weekday: 'long' });
  const dateDisplay = currentSelectedDate.toLocaleDateString('en-US', { month: 'long', day: 'numeric', year: 'numeric' });
  
  let html = `
    <div class="calendar-header">
      <button class="calendar-nav-btn" onclick="changeDay(-1)">‹</button>
      <div class="calendar-month-year">
        <span style="font-size:14pt">${dayName}</span>
        <span style="font-size:12pt;color:var(--text-secondary)">${dateDisplay}</span>
      </div>
      <button class="calendar-nav-btn" onclick="changeDay(1)">›</button>
    </div>
    <div class="day-view-container">
  `;
  
  // Create hourly slots
  for (let hour = 0; hour < 24; hour++) {
    const hourStr = String(hour).padStart(2, '0');
    const displayHour = hour === 0 ? '12 AM' : hour < 12 ? `${hour} AM` : hour === 12 ? '12 PM' : `${hour - 12} PM`;
    
    html += `
      <div class="day-hour-slot">
        <div class="day-hour-label">${displayHour}</div>
        <div class="day-hour-content">
    `;
    
    // Find events for this hour
    const hourEvents = events.filter(event => {
      if (!event.startTime) return false;
      const eventHour = parseInt(event.startTime.split(':')[0]);
      return eventHour === hour;
    });
    
    hourEvents.forEach(event => {
      const color = event.color || '#333';
      html += `
        <div class="day-event" style="border-left-color: ${color}">
          <div class="day-event-time">${formatTime(event.startTime)} - ${event.endTime ? formatTime(event.endTime) : ''}</div>
          <div class="day-event-title">${event.description}</div>
        </div>
      `;
    });
    
    html += `
        </div>
      </div>
    `;
  }
  
  html += '</div>';
  calendarWidget.innerHTML = html;
}

// Navigation functions for week and day views
function changeWeek(delta) {
  const newDate = new Date(currentSelectedDate);
  newDate.setDate(currentSelectedDate.getDate() + (delta * 7));
  currentSelectedDate = newDate;
  currentMonth = newDate.getMonth() + 1;
  currentYear = newDate.getFullYear();
  renderCalendar();
  updateEventList();
}

function changeDay(delta) {
  const newDate = new Date(currentSelectedDate);
  newDate.setDate(currentSelectedDate.getDate() + delta);
  currentSelectedDate = newDate;
  currentMonth = newDate.getMonth() + 1;
  currentYear = newDate.getFullYear();
  renderCalendar();
  updateEventList();
}

// Update selectDate to handle month/year parameters
function selectDateWithMonthYear(day, month, year) {
  currentSelectedDate = new Date(year, month - 1, day);
  currentMonth = month;
  currentYear = year;
  updateEventList();
  renderCalendar();
}

// Keyboard Shortcuts
document.addEventListener('keydown', (e) => {
  // Don't trigger shortcuts when typing in inputs
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA' || e.target.tagName === 'SELECT') {
    return;
  }
  
  // Check if calendar page is active
  if (!document.getElementById('calendarPage').classList.contains('active')) {
    return;
  }
  
  switch(e.key.toLowerCase()) {
    case 'n':
      // Focus on event input
      document.getElementById('eventLineEdit').focus();
      e.preventDefault();
      break;
      
    case 't':
      // Go to today
      goToToday();
      e.preventDefault();
      break;
      
    case 'arrowleft':
      // Navigate backward
      if (currentView === 'month') changeMonth(-1);
      else if (currentView === 'week') changeWeek(-1);
      else if (currentView === 'day') changeDay(-1);
      e.preventDefault();
      break;
      
    case 'arrowright':
      // Navigate forward
      if (currentView === 'month') changeMonth(1);
      else if (currentView === 'week') changeWeek(1);
      else if (currentView === 'day') changeDay(1);
      e.preventDefault();
      break;
      
    case 'arrowup':
      // Previous week in month view, previous day in week/day view
      if (currentView === 'month') {
        const newDate = new Date(currentSelectedDate);
        newDate.setDate(currentSelectedDate.getDate() - 7);
        currentSelectedDate = newDate;
        currentMonth = newDate.getMonth() + 1;
        currentYear = newDate.getFullYear();
        updateEventList();
        renderCalendar();
      } else {
        changeDay(-1);
      }
      e.preventDefault();
      break;
      
    case 'arrowdown':
      // Next week in month view, next day in week/day view
      if (currentView === 'month') {
        const newDate = new Date(currentSelectedDate);
        newDate.setDate(currentSelectedDate.getDate() + 7);
        currentSelectedDate = newDate;
        currentMonth = newDate.getMonth() + 1;
        currentYear = newDate.getFullYear();
        updateEventList();
        renderCalendar();
      } else {
        changeDay(1);
      }
      e.preventDefault();
      break;
      
    case '1':
      setCalendarView('month');
      e.preventDefault();
      break;
      
    case '2':
      setCalendarView('week');
      e.preventDefault();
      break;
      
    case '3':
      setCalendarView('day');
      e.preventDefault();
      break;
  }
});

// Go to Today function
function goToToday() {
  const today = new Date();
  currentSelectedDate = today;
  currentMonth = today.getMonth() + 1;
  currentYear = today.getFullYear();
  
  // Update selectors
  const monthSelector = document.getElementById("monthSelector");
  const yearDisplay = document.getElementById("yearDisplay");
  if (monthSelector) monthSelector.value = currentMonth;
  if (yearDisplay) yearDisplay.textContent = currentYear;
  
  updateEventList();
  renderCalendar();
}

// Override renderCalendar to support different views
const originalRenderCalendar = renderCalendar;
renderCalendar = async function() {
  if (currentView === 'week') {
    await renderWeekView();
  } else if (currentView === 'day') {
    await renderDayView();
  } else {
    await originalRenderCalendar();
  }
};

// Initialize view switcher
document.addEventListener('DOMContentLoaded', () => {
  // Always start with month view (calendar app should default to calendar)
  currentView = 'month';
  localStorage.setItem('calendarView', 'month');
  
  // Update active button based on current view
  document.querySelectorAll('.view-btn').forEach(btn => {
    btn.classList.toggle('active', btn.dataset.view === currentView);
  });
  
  // Add view switcher event listeners
  document.querySelectorAll('.view-btn').forEach(btn => {
    btn.addEventListener('click', () => {
      setCalendarView(btn.dataset.view);
    });
  });
  
  // Today button
  document.getElementById('goToTodayButton')?.addEventListener('click', goToToday);
});


// Keyboard Shortcuts Modal
function openShortcutsModal() {
  document.getElementById('shortcutsModalOverlay').classList.add('active');
}

function closeShortcutsModal() {
  document.getElementById('shortcutsModalOverlay').classList.remove('active');
}

// Add event listener for shortcuts button
document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('keyboardShortcutsBtn')?.addEventListener('click', openShortcutsModal);
  
  // Close shortcuts modal on overlay click
  document.getElementById('shortcutsModalOverlay')?.addEventListener('click', (e) => {
    if (e.target.id === 'shortcutsModalOverlay') closeShortcutsModal();
  });
});


// Category Filter
document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('categoryFilter')?.addEventListener('change', () => {
    updateEventList();
    renderCalendar();
  });
});


// Custom Delete Confirmation Modal
function showDeleteConfirmation(event) {
  const modal = document.getElementById('appModal');
  const overlay = document.getElementById('appModalOverlay');
  const message = document.getElementById('appModalMessage');
  const closeBtn = document.getElementById('appModalClose');
  
  message.textContent = 'Delete this event?';
  closeBtn.textContent = 'Cancel';
  closeBtn.style.background = 'var(--text-secondary)';
  
  // Create delete button
  const deleteBtn = document.createElement('button');
  deleteBtn.textContent = 'Delete';
  deleteBtn.className = 'btn btn-danger';
  deleteBtn.style.width = 'auto';
  deleteBtn.style.marginTop = '12px';
  deleteBtn.style.marginLeft = '10px';
  
  deleteBtn.onclick = async () => {
    const response = await fetch(`${API_BASE}/events/${event.id}`, { method: "DELETE" });
    const data = await response.json();
    
    if (data.success) {
      closeAppModal();
      updateEventList();
      renderCalendar();
      updateSearchSuggestions();
      document.getElementById("eventActions").style.display = "none";
      showAppMessage('Event deleted successfully!');
    } else {
      closeAppModal();
      showAppMessage('Could not delete event.');
    }
  };
  
  closeBtn.parentNode.appendChild(deleteBtn);
  overlay.style.display = 'flex';
  
  // Reset on close
  const originalClose = closeAppModal;
  window.closeAppModal = () => {
    if (deleteBtn.parentNode) deleteBtn.remove();
    closeBtn.textContent = 'OK';
    closeBtn.style.background = '';
    window.closeAppModal = originalClose;
    originalClose();
  };
}


// Loading Spinner Functions
function showLoading() {
  document.getElementById('loadingSpinner')?.classList.add('active');
  document.getElementById('loadingOverlay')?.classList.add('active');
}

function hideLoading() {
  document.getElementById('loadingSpinner')?.classList.remove('active');
  document.getElementById('loadingOverlay')?.classList.remove('active');
}

// Loading spinner is available via showLoading() and hideLoading()
// Use it manually for important operations like login


// Ripple Effect for Buttons
document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('.btn').forEach(button => {
    button.addEventListener('click', function(e) {
      const ripple = document.createElement('span');
      ripple.classList.add('ripple');
      
      const rect = this.getBoundingClientRect();
      const size = Math.max(rect.width, rect.height);
      const x = e.clientX - rect.left - size / 2;
      const y = e.clientY - rect.top - size / 2;
      
      ripple.style.width = ripple.style.height = size + 'px';
      ripple.style.left = x + 'px';
      ripple.style.top = y + 'px';
      
      this.appendChild(ripple);
      
      setTimeout(() => ripple.remove(), 600);
    });
  });
  
  // Remove preload class to enable transitions
  setTimeout(() => {
    document.body.classList.remove('preload');
  }, 100);
});


// ========== DASHBOARD VIEW ==========

const WEATHER_API_KEY = 'ac68617dbe4247b7af1113349251511'; // WeatherAPI.com key
const WEATHER_CITY = 'Karachi'; // Default city

// Motivational Quotes
const motivationalQuotes = [
  { text: "The future belongs to those who believe in the beauty of their dreams.", author: "Eleanor Roosevelt" },
  { text: "Success is not final, failure is not fatal: it is the courage to continue that counts.", author: "Winston Churchill" },
  { text: "Believe you can and you're halfway there.", author: "Theodore Roosevelt" },
  { text: "The only way to do great work is to love what you do.", author: "Steve Jobs" },
  { text: "Don't watch the clock; do what it does. Keep going.", author: "Sam Levenson" },
  { text: "The harder you work for something, the greater you'll feel when you achieve it.", author: "Anonymous" },
  { text: "Dream bigger. Do bigger.", author: "Anonymous" },
  { text: "Success doesn't just find you. You have to go out and get it.", author: "Anonymous" },
  { text: "Great things never come from comfort zones.", author: "Anonymous" },
  { text: "Dream it. Wish it. Do it.", author: "Anonymous" },
  { text: "Success is the sum of small efforts repeated day in and day out.", author: "Robert Collier" },
  { text: "Do something today that your future self will thank you for.", author: "Sean Patrick Flanery" }
];

let currentQuoteIndex = 0;

async function renderDashboardView() {
  const calendarWidget = document.getElementById("calendarWidget");
  
  let html = `
    <div class="dashboard-container">
      <!-- Today's Agenda -->
      <div class="dashboard-widget">
        <div class="widget-header">
          <span class="widget-icon">📋</span>
          <span class="widget-title">Today's Agenda</span>
        </div>
        <div class="widget-content" id="todayAgenda">
          <div class="agenda-empty">Loading...</div>
        </div>
      </div>
      
      <!-- Upcoming Events -->
      <div class="dashboard-widget">
        <div class="widget-header">
          <span class="widget-icon">📅</span>
          <span class="widget-title">Upcoming Events</span>
        </div>
        <div class="widget-content" id="upcomingEvents">
          <div class="agenda-empty">Loading...</div>
        </div>
      </div>
      
      <!-- Statistics -->
      <div class="dashboard-widget">
        <div class="widget-header">
          <span class="widget-icon">📊</span>
          <span class="widget-title">Statistics</span>
        </div>
        <div class="widget-content">
          <div class="stats-grid" id="statsGrid">
            <div class="stat-card">
              <div class="stat-number">-</div>
              <div class="stat-label">This Week</div>
            </div>
            <div class="stat-card">
              <div class="stat-number">-</div>
              <div class="stat-label">This Month</div>
            </div>
            <div class="stat-card">
              <div class="stat-number">-</div>
              <div class="stat-label">Total Events</div>
            </div>
            <div class="stat-card">
              <div class="stat-number">-</div>
              <div class="stat-label">Upcoming</div>
            </div>
          </div>
        </div>
      </div>
      
      <!-- Weather -->
      <div class="dashboard-widget">
        <div class="widget-header">
          <span class="widget-icon">🌤️</span>
          <span class="widget-title">Weather</span>
        </div>
        <div class="widget-content">
          <div style="display:flex;gap:10px;margin-bottom:15px;">
            <select id="citySearchInput" style="flex:1;margin:0;padding:8px 12px;border-radius:8px;border:2px solid var(--border);font-size:10pt;background:var(--bg-surface);color:var(--text-primary);cursor:pointer;">
              <option value="Karachi">Karachi</option>
              <option value="Lahore">Lahore</option>
              <option value="Islamabad">Islamabad</option>
              <option value="Rawalpindi">Rawalpindi</option>
              <option value="Faisalabad">Faisalabad</option>
              <option value="Multan">Multan</option>
              <option value="Peshawar">Peshawar</option>
              <option value="Quetta">Quetta</option>
              <option value="Sialkot">Sialkot</option>
              <option value="Gujranwala">Gujranwala</option>
              <option value="Hyderabad">Hyderabad</option>
              <option value="Sukkur">Sukkur</option>
              <option value="Bahawalpur">Bahawalpur</option>
              <option value="Sargodha">Sargodha</option>
              <option value="Abbottabad">Abbottabad</option>
              <option value="Mardan">Mardan</option>
              <option value="Gujrat">Gujrat</option>
              <option value="Larkana">Larkana</option>
              <option value="Sheikhupura">Sheikhupura</option>
              <option value="Rahim Yar Khan">Rahim Yar Khan</option>
            </select>
            <button onclick="window.searchWeatherCity()" style="padding:8px 16px;background:var(--brand);color:white;border:none;border-radius:8px;cursor:pointer;font-size:10pt;font-weight:600;">🔍</button>
          </div>
          <div id="weatherWidget">
            <div class="weather-loading">Loading weather...</div>
          </div>
        </div>
      </div>
      
      <!-- Motivational Quote -->
      <div class="dashboard-widget" style="grid-column: span 2;">
        <div class="widget-header">
          <span class="widget-icon">💭</span>
          <span class="widget-title">Motivation</span>
        </div>
        <div class="widget-content" id="quoteWidget">
          <div class="quote-text"></div>
          <div class="quote-author"></div>
          <button class="quote-refresh" onclick="rotateQuote()">🔄 New Quote</button>
        </div>
      </div>
    </div>
  `;
  
  calendarWidget.innerHTML = html;
  
  // Add Enter key listener for city search
  setTimeout(() => {
    const cityInput = document.getElementById('citySearchInput');
    if (cityInput) {
      cityInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
          e.preventDefault();
          window.searchWeatherCity();
        }
      });
    }
  }, 100);
  
  // Load dashboard data
  await loadTodayAgenda();
  await loadUpcomingEvents();
  await loadStatistics();
  await loadWeather();
  rotateQuote();
}

// Load Today's Agenda
async function loadTodayAgenda() {
  const today = new Date();
  const dateStr = formatDate(today);
  const response = await fetch(`${API_BASE}/events?username=${currentUsername}&date=${dateStr}`);
  const data = await response.json();
  
  const container = document.getElementById('todayAgenda');
  const events = data.events || [];
  
  if (events.length === 0) {
    container.innerHTML = '<div class="agenda-empty">No events today. Enjoy your free time! 🎉</div>';
    return;
  }
  
  let html = '';
  events.forEach(event => {
    const categoryIcon = event.categoryIcon || '';
    html += `
      <div class="agenda-item">
        <div class="agenda-time">${event.startTime ? formatTime(event.startTime) : 'All Day'}</div>
        <div class="agenda-description">${categoryIcon} ${event.description}</div>
      </div>
    `;
  });
  
  container.innerHTML = html;
}

// Load Upcoming Events
async function loadUpcomingEvents() {
  const response = await fetch(`${API_BASE}/events/all?username=${currentUsername}`);
  const data = await response.json();
  
  const container = document.getElementById('upcomingEvents');
  const allEvents = data.events || [];
  
  // Filter future events
  const today = new Date();
  today.setHours(0, 0, 0, 0);
  
  const upcomingEvents = allEvents
    .filter(event => new Date(event.date) > today)
    .slice(0, 5);
  
  if (upcomingEvents.length === 0) {
    container.innerHTML = '<div class="agenda-empty">No upcoming events</div>';
    return;
  }
  
  let html = '';
  upcomingEvents.forEach(event => {
    const eventDate = new Date(event.date);
    const dateStr = eventDate.toLocaleDateString('en-US', { month: 'short', day: 'numeric' });
    const categoryIcon = event.categoryIcon || '';
    
    html += `
      <div class="upcoming-event">
        <div>
          <div class="upcoming-title">${categoryIcon} ${event.description}</div>
          <div class="upcoming-date">${dateStr}</div>
        </div>
      </div>
    `;
  });
  
  container.innerHTML = html;
}

// Load Statistics
async function loadStatistics() {
  const response = await fetch(`${API_BASE}/events/all?username=${currentUsername}`);
  const data = await response.json();
  const allEvents = data.events || [];
  
  const today = new Date();
  today.setHours(0, 0, 0, 0);
  
  // This week
  const startOfWeek = new Date(today);
  startOfWeek.setDate(today.getDate() - today.getDay());
  const endOfWeek = new Date(startOfWeek);
  endOfWeek.setDate(startOfWeek.getDate() + 6);
  
  const thisWeek = allEvents.filter(event => {
    const eventDate = new Date(event.date);
    return eventDate >= startOfWeek && eventDate <= endOfWeek;
  }).length;
  
  // This month
  const thisMonth = allEvents.filter(event => {
    const eventDate = new Date(event.date);
    return eventDate.getMonth() === today.getMonth() && eventDate.getFullYear() === today.getFullYear();
  }).length;
  
  // Total events
  const total = allEvents.length;
  
  // Upcoming events
  const upcoming = allEvents.filter(event => new Date(event.date) > today).length;
  
  const container = document.getElementById('statsGrid');
  container.innerHTML = `
    <div class="stat-card">
      <div class="stat-number">${thisWeek}</div>
      <div class="stat-label">This Week</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">${thisMonth}</div>
      <div class="stat-label">This Month</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">${total}</div>
      <div class="stat-label">Total Events</div>
    </div>
    <div class="stat-card">
      <div class="stat-number">${upcoming}</div>
      <div class="stat-label">Upcoming</div>
    </div>
  `;
}

// Load Weather using WeatherAPI.com
async function loadWeather(city = WEATHER_CITY) {
  const container = document.getElementById('weatherWidget');
  
  try {
    // WeatherAPI.com endpoint
    const response = await fetch(
      `https://api.weatherapi.com/v1/current.json?key=${WEATHER_API_KEY}&q=${city},Pakistan&aqi=no`
    );
    
    if (!response.ok) {
      throw new Error('Weather API error');
    }
    
    const data = await response.json();
    
    if (data.error) {
      container.innerHTML = '<div class="weather-loading">City not found</div>';
      return;
    }
    
    const temp = Math.round(data.current.temp_c);
    const description = data.current.condition.text;
    const humidity = data.current.humidity;
    const windSpeed = Math.round(data.current.wind_kph);
    const feelsLike = Math.round(data.current.feelslike_c);
    const isDay = data.current.is_day === 1;
    
    // Weather icon mapping based on condition code
    const getWeatherIcon = (code, isDay) => {
      if (code === 1000) return isDay ? '☀️' : '🌙'; // Clear
      if (code === 1003) return isDay ? '⛅' : '☁️'; // Partly cloudy
      if ([1006, 1009].includes(code)) return '☁️'; // Cloudy
      if ([1063, 1180, 1183, 1186, 1189, 1192, 1195, 1240, 1243, 1246].includes(code)) return '🌧️'; // Rain
      if ([1087, 1273, 1276].includes(code)) return '⛈️'; // Thunderstorm
      if ([1066, 1210, 1213, 1216, 1219, 1222, 1225, 1255, 1258].includes(code)) return '❄️'; // Snow
      if ([1030, 1135, 1147].includes(code)) return '🌫️'; // Fog/Mist
      return '🌤️'; // Default
    };
    
    const weatherIcon = getWeatherIcon(data.current.condition.code, isDay);
    
    container.innerHTML = `
      <div class="weather-main">
        <div>
          <div class="weather-temp">${temp}°C</div>
          <div class="weather-description">${description}</div>
        </div>
        <div class="weather-icon">${weatherIcon}</div>
      </div>
      <div class="weather-details">
        <div class="weather-detail">
          <div class="weather-detail-label">Feels Like</div>
          <div class="weather-detail-value">${feelsLike}°C</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Humidity</div>
          <div class="weather-detail-value">${humidity}%</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Wind Speed</div>
          <div class="weather-detail-value">${windSpeed} km/h</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Location</div>
          <div class="weather-detail-value">${data.location.name}</div>
        </div>
      </div>
    `;
  } catch (error) {
    // Fallback weather display
    container.innerHTML = `
      <div class="weather-main">
        <div>
          <div class="weather-temp">28°C</div>
          <div class="weather-description">Partly Cloudy</div>
        </div>
        <div class="weather-icon">⛅</div>
      </div>
      <div class="weather-details">
        <div class="weather-detail">
          <div class="weather-detail-label">Feels Like</div>
          <div class="weather-detail-value">30°C</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Humidity</div>
          <div class="weather-detail-value">65%</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Wind Speed</div>
          <div class="weather-detail-value">15 km/h</div>
        </div>
        <div class="weather-detail">
          <div class="weather-detail-label">Location</div>
          <div class="weather-detail-value">${WEATHER_CITY}</div>
        </div>
      </div>

    `;
  }
}

// Rotate Quote
function rotateQuote() {
  const quote = motivationalQuotes[currentQuoteIndex];
  document.querySelector('.quote-text').textContent = `"${quote.text}"`;
  document.querySelector('.quote-author').textContent = `— ${quote.author}`;
  
  currentQuoteIndex = (currentQuoteIndex + 1) % motivationalQuotes.length;
}

// Update renderCalendar to support dashboard
const originalRenderCalendarFunc = renderCalendar;
renderCalendar = async function() {
  if (currentView === 'dashboard') {
    await renderDashboardView();
  } else if (currentView === 'week') {
    await renderWeekView();
  } else if (currentView === 'day') {
    await renderDayView();
  } else {
    await originalRenderCalendarFunc();
  }
};


// Enter key to submit login forms
document.addEventListener('DOMContentLoaded', () => {
  // Personal Login
  const personalUsername = document.getElementById('personalUsername');
  const personalPassword = document.getElementById('personalPassword');
  
  [personalUsername, personalPassword].forEach(input => {
    input?.addEventListener('keypress', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        document.getElementById('personalLoginButton').click();
      }
    });
  });
  
  // Admin Login
  const adminUsername = document.getElementById('adminUsername');
  const adminPassword = document.getElementById('adminPassword');
  
  [adminUsername, adminPassword].forEach(input => {
    input?.addEventListener('keypress', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        document.getElementById('adminLoginButton').click();
      }
    });
  });
  
  // Student Login
  const studentName = document.getElementById('studentNameLineEdit');
  const studentRoll = document.getElementById('studentRollLineEdit');
  const studentPassword = document.getElementById('studentPasswordLineEdit');
  
  [studentName, studentRoll, studentPassword].forEach(input => {
    input?.addEventListener('keypress', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        document.getElementById('studentLoginButton').click();
      }
    });
  });
  
  // Teacher Login
  const teacherUsername = document.getElementById('teacherUsernameLineEdit');
  const teacherPassword = document.getElementById('teacherPasswordLineEdit');
  
  [teacherUsername, teacherPassword].forEach(input => {
    input?.addEventListener('keypress', (e) => {
      if (e.key === 'Enter') {
        e.preventDefault();
        document.getElementById('teacherLoginButton').click();
      }
    });
  });
});


// Custom Logout Confirmation Modal
function showLogoutConfirmation() {
  const modal = document.getElementById('appModal');
  const overlay = document.getElementById('appModalOverlay');
  const message = document.getElementById('appModalMessage');
  const closeBtn = document.getElementById('appModalClose');
  
  message.textContent = 'Do you want to logout?';
  closeBtn.textContent = 'Cancel';
  closeBtn.style.background = 'var(--text-secondary)';
  
  // Create logout button
  const logoutBtn = document.createElement('button');
  logoutBtn.textContent = 'Logout';
  logoutBtn.className = 'btn btn-danger';
  logoutBtn.style.width = 'auto';
  logoutBtn.style.marginTop = '12px';
  logoutBtn.style.marginLeft = '10px';
  
  logoutBtn.onclick = () => {
    closeAppModal();
    document.getElementById('backToLoginButton').click();
  };
  
  closeBtn.parentNode.appendChild(logoutBtn);
  overlay.style.display = 'flex';
  
  // Keyboard navigation for modal buttons
  let focusedButton = 0; // 0 = Cancel, 1 = Logout
  
  const updateButtonFocus = () => {
    if (focusedButton === 0) {
      closeBtn.style.outline = '3px solid var(--brand)';
      closeBtn.style.outlineOffset = '2px';
      logoutBtn.style.outline = 'none';
    } else {
      closeBtn.style.outline = 'none';
      logoutBtn.style.outline = '3px solid var(--brand)';
      logoutBtn.style.outlineOffset = '2px';
    }
  };
  
  // Set initial focus on Cancel
  updateButtonFocus();
  
  const keyHandler = (e) => {
    if (e.key === 'ArrowLeft') {
      focusedButton = 0; // Cancel
      updateButtonFocus();
      e.preventDefault();
    } else if (e.key === 'ArrowRight') {
      focusedButton = 1; // Logout
      updateButtonFocus();
      e.preventDefault();
    } else if (e.key === 'Enter') {
      if (focusedButton === 0) {
        closeBtn.click();
      } else {
        logoutBtn.click();
      }
      e.preventDefault();
    }
  };
  
  document.addEventListener('keydown', keyHandler);
  
  // Reset on close
  const originalClose = closeAppModal;
  window.closeAppModal = () => {
    document.removeEventListener('keydown', keyHandler);
    if (logoutBtn.parentNode) logoutBtn.remove();
    closeBtn.textContent = 'OK';
    closeBtn.style.background = '';
    closeBtn.style.outline = 'none';
    window.closeAppModal = originalClose;
    originalClose();
  };
}





// Global function for weather city search
window.searchWeatherCity = async function() {
  const cityInput = document.getElementById('citySearchInput');
  if (!cityInput) {
    console.error('City input not found');
    return;
  }
  const city = cityInput.value.trim();
  console.log('Searching weather for:', city);
  if (city) {
    await loadWeather(city);
  } else {
    showAppMessage('Please enter a city name');
  }
};
