"""
Data Structures and Algorithms implementations for Time Weaver
Converted from C++ to Python
"""

from typing import Dict, List, Optional, Tuple
from collections import deque
import re
from datetime import datetime, timedelta


class TrieNode:
    """Node for Trie data structure"""
    def __init__(self):
        self.children: Dict[str, 'TrieNode'] = {}
        self.event_ids: List[int] = []
        self.is_end_of_word: bool = False


class EventSearchTrie:
    """Trie data structure for fast event search"""
    
    def __init__(self):
        self.root = TrieNode()
    
    def insert(self, keyword: str, event_id: int) -> None:
        """Insert a keyword into the trie"""
        keyword = keyword.lower()
        current = self.root
        
        for char in keyword:
            if char not in current.children:
                current.children[char] = TrieNode()
            current = current.children[char]
        
        current.is_end_of_word = True
        if event_id not in current.event_ids:
            current.event_ids.append(event_id)
    
    def search(self, prefix: str) -> List[int]:
        """Search for all event IDs matching the prefix"""
        prefix = prefix.lower()
        current = self.root
        
        for char in prefix:
            if char not in current.children:
                return []
            current = current.children[char]
        
        results = []
        self._collect_all_event_ids(current, results)
        return results
    
    def _collect_all_event_ids(self, node: TrieNode, results: List[int]) -> None:
        """Recursively collect all event IDs from a node"""
        if node.is_end_of_word:
            results.extend(node.event_ids)
        for child in node.children.values():
            self._collect_all_event_ids(child, results)


class Event:
    """Event data structure"""
    def __init__(self):
        self.id: int = 0
        self.username: str = ""
        self.date: str = ""
        self.description: str = ""
        self.start_time: str = ""
        self.end_time: str = ""
        self.color: str = ""
        self.is_personal: bool = False
        self.recurrence: str = ""
        self.category: str = ""
        self.category_icon: str = ""
        self.department: str = ""


class EventScheduler:
    """Priority queue-based event scheduler"""
    
    def __init__(self):
        self.events: List[Event] = []
    
    def add_event(self, event: Event) -> None:
        """Add an event to the scheduler"""
        self.events.append(event)
        self.events.sort(key=lambda e: (e.date, e.start_time))
    
    def get_events_for_date(self, date: str) -> List[Event]:
        """Get all events for a specific date"""
        result = [e for e in self.events if e.date == date]
        result.sort(key=lambda e: e.start_time)
        return result
    
    def get_next_event(self) -> Optional[Event]:
        """Get the next upcoming event"""
        if not self.events:
            return None
        return self.events[0]
    
    def is_empty(self) -> bool:
        """Check if scheduler is empty"""
        return len(self.events) == 0


class LRUNode:
    """Node for LRU Cache"""
    def __init__(self, key: str, value: str):
        self.key = key
        self.value = value
        self.prev: Optional['LRUNode'] = None
        self.next: Optional['LRUNode'] = None


class LRUCache:
    """LRU Cache implementation"""
    
    def __init__(self, capacity: int):
        self.capacity = capacity
        self.cache: Dict[str, LRUNode] = {}
        self.head = LRUNode("", "")
        self.tail = LRUNode("", "")
        self.head.next = self.tail
        self.tail.prev = self.head
    
    def _add_to_front(self, node: LRUNode) -> None:
        """Add node to the front of the list"""
        node.next = self.head.next
        node.prev = self.head
        self.head.next.prev = node
        self.head.next = node
    
    def _remove_node(self, node: LRUNode) -> None:
        """Remove node from the list"""
        node.prev.next = node.next
        node.next.prev = node.prev
    
    def _move_to_front(self, node: LRUNode) -> None:
        """Move node to the front"""
        self._remove_node(node)
        self._add_to_front(node)
    
    def get(self, key: str) -> str:
        """Get value from cache"""
        if key not in self.cache:
            return ""
        node = self.cache[key]
        self._move_to_front(node)
        return node.value
    
    def put(self, key: str, value: str) -> None:
        """Put value into cache"""
        if key in self.cache:
            node = self.cache[key]
            node.value = value
            self._move_to_front(node)
            return
        
        if len(self.cache) >= self.capacity:
            lru = self.tail.prev
            self._remove_node(lru)
            del self.cache[lru.key]
        
        new_node = LRUNode(key, value)
        self.cache[key] = new_node
        self._add_to_front(new_node)


class RecurrenceEngine:
    """Recurrence pattern matching engine"""
    
    class RecurrenceType:
        NONE = 0
        DAILY = 1
        WEEKLY = 2
        MONTHLY = 3
        YEARLY = 4
    
    @staticmethod
    def string_to_type(recurrence_str: str) -> int:
        """Convert string to recurrence type"""
        recurrence_str = recurrence_str.lower()
        if recurrence_str == "daily":
            return RecurrenceEngine.RecurrenceType.DAILY
        elif recurrence_str == "weekly":
            return RecurrenceEngine.RecurrenceType.WEEKLY
        elif recurrence_str == "monthly":
            return RecurrenceEngine.RecurrenceType.MONTHLY
        elif recurrence_str == "yearly":
            return RecurrenceEngine.RecurrenceType.YEARLY
        return RecurrenceEngine.RecurrenceType.NONE
    
    @staticmethod
    def generate_recurring_dates(start_date: str, rec_type: int, count: int = 365) -> List[str]:
        """Generate recurring dates"""
        dates = []
        if rec_type == RecurrenceEngine.RecurrenceType.NONE:
            dates.append(start_date)
            return dates
        
        try:
            dt = datetime.strptime(start_date, "%Y-%m-%d")
        except ValueError:
            return dates
        
        for i in range(count):
            dates.append(dt.strftime("%Y-%m-%d"))
            
            if rec_type == RecurrenceEngine.RecurrenceType.DAILY:
                dt += timedelta(days=1)
            elif rec_type == RecurrenceEngine.RecurrenceType.WEEKLY:
                dt += timedelta(weeks=1)
            elif rec_type == RecurrenceEngine.RecurrenceType.MONTHLY:
                # Add one month
                if dt.month == 12:
                    dt = dt.replace(year=dt.year + 1, month=1)
                else:
                    dt = dt.replace(month=dt.month + 1)
            elif rec_type == RecurrenceEngine.RecurrenceType.YEARLY:
                dt = dt.replace(year=dt.year + 1)
        
        return dates
    
    @staticmethod
    def matches_recurrence(event_date: str, target_date: str, rec_type: int) -> bool:
        """Check if target date matches recurrence pattern"""
        if rec_type == RecurrenceEngine.RecurrenceType.NONE:
            return event_date == target_date
        
        try:
            event_dt = datetime.strptime(event_date, "%Y-%m-%d")
            target_dt = datetime.strptime(target_date, "%Y-%m-%d")
        except ValueError:
            return False
        
        if target_dt < event_dt:
            return False
        
        if rec_type == RecurrenceEngine.RecurrenceType.DAILY:
            return True
        elif rec_type == RecurrenceEngine.RecurrenceType.WEEKLY:
            return event_dt.weekday() == target_dt.weekday()
        elif rec_type == RecurrenceEngine.RecurrenceType.MONTHLY:
            return event_dt.day == target_dt.day
        elif rec_type == RecurrenceEngine.RecurrenceType.YEARLY:
            return event_dt.month == target_dt.month and event_dt.day == target_dt.day
        
        return False


class BSTNode:
    """Node for Binary Search Tree"""
    def __init__(self, time: str, event_id: int):
        self.time = time
        self.event_ids: List[int] = [event_id]
        self.left: Optional['BSTNode'] = None
        self.right: Optional['BSTNode'] = None


class EventBST:
    """Binary Search Tree for time-based event queries"""
    
    def __init__(self):
        self.root: Optional[BSTNode] = None
    
    def insert(self, time: str, event_id: int) -> None:
        """Insert a time slot into the BST"""
        self.root = self._insert(self.root, time, event_id)
    
    def _insert(self, node: Optional[BSTNode], time: str, event_id: int) -> BSTNode:
        """Recursive insert"""
        if node is None:
            return BSTNode(time, event_id)
        
        if time < node.time:
            node.left = self._insert(node.left, time, event_id)
        elif time > node.time:
            node.right = self._insert(node.right, time, event_id)
        else:
            if event_id not in node.event_ids:
                node.event_ids.append(event_id)
        
        return node
    
    def find_events_in_range(self, start: str, end: str) -> List[int]:
        """Find all events in a time range"""
        result = []
        self._find_in_range(self.root, start, end, result)
        return result
    
    def _find_in_range(self, node: Optional[BSTNode], start: str, end: str, result: List[int]) -> None:
        """Recursive range search"""
        if node is None:
            return
        
        if node.time > start:
            self._find_in_range(node.left, start, end, result)
        
        if start <= node.time <= end:
            result.extend(node.event_ids)
        
        if node.time < end:
            self._find_in_range(node.right, start, end, result)
    
    def find_conflicts(self, time: str) -> List[int]:
        """Find events at a specific time"""
        return self.find_events_in_range(time, time)

