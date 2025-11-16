// Academic Features: Teacher Department Events, Availability, Student Teacher Search
// This file contains handlers for the new academic modals and shortcuts

// ============================================================================
// TEACHER: Department Event Modal
// ============================================================================
function openDeptEventModal() {
  document.getElementById('teacherDeptEventModalOverlay').classList.add('active');
  // Set default date to today
  const today = new Date().toISOString().split('T')[0];
  document.getElementById('deptEventDate').value = today;
}

function closeDeptEventModal() {
  document.getElementById('teacherDeptEventModalOverlay').classList.remove('active');
}

document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('teacherDepartmentEventButton')?.addEventListener('click', openDeptEventModal);
  
  document.getElementById('postDeptEventButton')?.addEventListener('click', async () => {
    const description = document.getElementById('deptEventDescription').value.trim();
    const date = document.getElementById('deptEventDate').value;
    const startTime = document.getElementById('deptEventStartTime').value;
    const endTime = document.getElementById('deptEventEndTime').value;
    const colorIndex = document.getElementById('deptEventColor').selectedIndex;
    
    if (!description || !date) {
      showAppMessage('Please fill in event description and date.');
      return;
    }
    
    if (!currentUserDepartment) {
      showAppMessage('Department not set. Please re-login.');
      return;
    }
    
    const colorNames = ["", "Red", "Green", "Blue", "Yellow", "Orange", "Purple"];
    const color = colorNames[colorIndex];
    
    const response = await fetch(`${API_BASE}/events`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        username: currentUsername,
        date,
        description,
        startTime: startTime || "",
        endTime: endTime || "",
        color,
        isPersonal: false,
        recurrence: 'none',
        category: '',
        categoryIcon: '',
        department: currentUserDepartment
      })
    });
    
    const data = await response.json();
    if (data.success) {
      showAppMessage('Department event posted successfully!');
      document.getElementById('deptEventDescription').value = '';
      document.getElementById('deptEventColor').selectedIndex = 0;
      closeDeptEventModal();
      updateEventList();
      renderCalendar();
    } else {
      showAppMessage('Failed to post department event.');
    }
  });
  
  // Close on overlay click
  document.getElementById('teacherDeptEventModalOverlay')?.addEventListener('click', (e) => {
    if (e.target.id === 'teacherDeptEventModalOverlay') closeDeptEventModal();
  });
});

// ============================================================================
// TEACHER: Availability Modal
// ============================================================================
function openAvailabilityModal() {
  document.getElementById('teacherAvailabilityModalOverlay').classList.add('active');
  loadTeacherAvailability();
}

function closeAvailabilityModal() {
  document.getElementById('teacherAvailabilityModalOverlay').classList.remove('active');
}

async function loadTeacherAvailability() {
  const response = await fetch(`${API_BASE}/teachers/availability?teacher=${encodeURIComponent(currentUsername)}`);
  const data = await response.json();
  
  const list = document.getElementById('availabilitySlotsList');
  list.innerHTML = '';
  
  if (!data.slots || data.slots.length === 0) {
    list.innerHTML = '<p style="color:var(--text-secondary);font-size:10pt;">No availability slots set.</p>';
    return;
  }
  
  const dayNames = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
  data.slots.forEach(slot => {
    const div = document.createElement('div');
    div.className = 'event-item';
    div.style.fontSize = '10pt';
    div.textContent = `${dayNames[slot.dayOfWeek]}: ${slot.startTime} - ${slot.endTime}`;
    if (slot.note) div.textContent += ` (${slot.note})`;
    list.appendChild(div);
  });
}

document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('teacherAvailabilityButton')?.addEventListener('click', openAvailabilityModal);
  
  document.getElementById('addAvailabilitySlotButton')?.addEventListener('click', async () => {
    const day = document.getElementById('availabilityDay').value;
    const startTime = document.getElementById('availabilityStartTime').value;
    const endTime = document.getElementById('availabilityEndTime').value;
    const note = document.getElementById('availabilityNote').value.trim();
    
    if (!day || !startTime || !endTime) {
      showAppMessage('Please select day and times.');
      return;
    }
    
    const response = await fetch(`${API_BASE}/teachers/availability`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        username: currentUsername,
        dayOfWeek: day,
        startTime,
        endTime,
        note,
        clearExisting: 'false'
      })
    });
    
    const data = await response.json();
    if (data.success) {
      showAppMessage('Availability slot added!');
      document.getElementById('availabilityNote').value = '';
      loadTeacherAvailability();
    } else {
      showAppMessage('Failed to add availability slot.');
    }
  });
  
  document.getElementById('clearAvailabilityButton')?.addEventListener('click', async () => {
    if (!confirm('Clear all your availability slots?')) return;
    
    const response = await fetch(`${API_BASE}/teachers/availability`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        username: currentUsername,
        dayOfWeek: '0',
        startTime: '00:00',
        endTime: '00:00',
        note: '',
        clearExisting: 'true'
      })
    });
    
    const data = await response.json();
    if (data.success) {
      showAppMessage('All availability slots cleared.');
      loadTeacherAvailability();
    } else {
      showAppMessage('Failed to clear availability.');
    }
  });
  
  // Close on overlay click
  document.getElementById('teacherAvailabilityModalOverlay')?.addEventListener('click', (e) => {
    if (e.target.id === 'teacherAvailabilityModalOverlay') closeAvailabilityModal();
  });
});

// ============================================================================
// STUDENT: Find Teacher Modal
// ============================================================================
function openFindTeacherModal() {
  document.getElementById('studentFindTeacherModalOverlay').classList.add('active');
  document.getElementById('teacherSearchResults').innerHTML = '';
}

function closeFindTeacherModal() {
  document.getElementById('studentFindTeacherModalOverlay').classList.remove('active');
}

document.addEventListener('DOMContentLoaded', () => {
  document.getElementById('studentFindTeacherButton')?.addEventListener('click', openFindTeacherModal);
  
  document.getElementById('searchTeacherButton')?.addEventListener('click', async () => {
    const teacherName = document.getElementById('findTeacherName').value.trim();
    
    if (!teacherName) {
      showAppMessage('Please enter a teacher username.');
      return;
    }
    
    // Search for teacher (filtered by student's university and department)
    const searchResponse = await fetch(`${API_BASE}/teachers/search?name=${encodeURIComponent(teacherName)}&university=${encodeURIComponent(currentUserUniversity)}&department=${encodeURIComponent(currentUserDepartment)}`);
    const searchData = await searchResponse.json();
    
    const resultsDiv = document.getElementById('teacherSearchResults');
    resultsDiv.innerHTML = '';
    
    if (!searchData.teachers || searchData.teachers.length === 0) {
      const deptName = typeof DEPARTMENTS !== 'undefined' && DEPARTMENTS[currentUserDepartment] ? DEPARTMENTS[currentUserDepartment] : currentUserDepartment;
      resultsDiv.innerHTML = `<p style="color:var(--text-secondary);font-size:10pt;">No teachers found in ${deptName} department at ${currentUserUniversity}.<br><br>Search is limited to your university and department.</p>`;
      return;
    }
    
    // For each teacher, fetch availability
    for (const teacher of searchData.teachers) {
      const teacherDiv = document.createElement('div');
      teacherDiv.style.marginBottom = '20px';
      teacherDiv.style.padding = '15px';
      teacherDiv.style.background = 'var(--bg-surface)';
      teacherDiv.style.borderRadius = '10px';
      teacherDiv.style.border = '1px solid var(--border)';
      
      const nameHeader = document.createElement('h3');
      nameHeader.textContent = `${teacher.displayName} (@${teacher.username})`;
      nameHeader.style.marginBottom = '5px';
      nameHeader.style.color = 'var(--text-primary)';
      teacherDiv.appendChild(nameHeader);
      
      const deptInfo = document.createElement('p');
      deptInfo.textContent = `${teacher.department} - ${teacher.university}`;
      deptInfo.style.fontSize = '9pt';
      deptInfo.style.color = 'var(--text-secondary)';
      deptInfo.style.marginBottom = '10px';
      teacherDiv.appendChild(deptInfo);
      
      // Fetch availability
      const availResponse = await fetch(`${API_BASE}/teachers/availability?teacher=${encodeURIComponent(teacher.username)}`);
      const availData = await availResponse.json();
      
      if (!availData.slots || availData.slots.length === 0) {
        const noAvail = document.createElement('p');
        noAvail.textContent = 'No availability set.';
        noAvail.style.fontSize = '10pt';
        noAvail.style.color = 'var(--text-secondary)';
        teacherDiv.appendChild(noAvail);
      } else {
        const availTitle = document.createElement('p');
        availTitle.textContent = 'Available:';
        availTitle.style.fontWeight = 'bold';
        availTitle.style.marginBottom = '5px';
        availTitle.style.fontSize = '10pt';
        teacherDiv.appendChild(availTitle);
        
        const dayNames = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
        availData.slots.forEach(slot => {
          const slotDiv = document.createElement('div');
          slotDiv.style.fontSize = '9pt';
          slotDiv.style.padding = '5px 10px';
          slotDiv.style.background = 'var(--bg-primary)';
          slotDiv.style.borderRadius = '5px';
          slotDiv.style.marginBottom = '5px';
          slotDiv.textContent = `${dayNames[slot.dayOfWeek]}: ${slot.startTime} - ${slot.endTime}`;
          if (slot.note) slotDiv.textContent += ` • ${slot.note}`;
          teacherDiv.appendChild(slotDiv);
        });
      }
      
      resultsDiv.appendChild(teacherDiv);
    }
  });
  
  // Close on overlay click
  document.getElementById('studentFindTeacherModalOverlay')?.addEventListener('click', (e) => {
    if (e.target.id === 'studentFindTeacherModalOverlay') closeFindTeacherModal();
  });
});

// ============================================================================
// KEYBOARD SHORTCUTS for Academic Features
// ============================================================================
document.addEventListener('keydown', (e) => {
  // Don't trigger shortcuts when typing in inputs
  if (e.target.tagName === 'INPUT' || e.target.tagName === 'TEXTAREA' || e.target.tagName === 'SELECT') {
    return;
  }
  
  // Check if calendar page is active
  if (!document.getElementById('calendarPage')?.classList.contains('active')) {
    return;
  }
  
  const key = e.key.toLowerCase();
  
  // Teacher shortcuts
  if (currentUserRole === 'teacher') {
    if (key === 'd') {
      openDeptEventModal();
      e.preventDefault();
    } else if (key === 'a') {
      openAvailabilityModal();
      e.preventDefault();
    }
  }
  
  // Student shortcuts
  if (currentUserRole === 'student') {
    if (key === 'f') {
      openFindTeacherModal();
      e.preventDefault();
    }
  }
});

// ============================================================================
// Update shortcuts modal visibility based on role
// ============================================================================
function updateShortcutsModal() {
  const teacherSection = document.getElementById('teacherShortcuts');
  const studentSection = document.getElementById('studentShortcuts');
  
  if (teacherSection) {
    teacherSection.style.display = currentUserRole === 'teacher' ? 'block' : 'none';
  }
  if (studentSection) {
    studentSection.style.display = currentUserRole === 'student' ? 'block' : 'none';
  }
}

// Call this when calendar page is set up
const originalSetupCalendarPage = setupCalendarPage;
setupCalendarPage = function() {
  originalSetupCalendarPage();
  updateShortcutsModal();
};
