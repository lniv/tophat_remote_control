"""
take an xci file, a list of possible keys, and create a derived file where for each mode the keys that are unused are assigned to a null event.
The point is to ensure we don't have previously defined key/event assignments pop up without our knowledge.
One could do something of the sort by using the #CLEAN directive at the start of the xci file, but then we seem to lose lots of stuff defined directly in the C code that i can't recreate in the pure xci file
"""
