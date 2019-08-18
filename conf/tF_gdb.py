class Int2Printer:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "{%d,%d}" % (self.val['x'], self.val['y'])


class RangePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "{%s...%s}" % (self.val['start'], self.val['end'])


class RectPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "{pos = %s, size = %s}" % (self.val['pos'], self.val['size'])


class StringPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return "{chars = " + str(self.val['chars']['i']['data']) + \
            ", size = %d" % int(self.val['chars']['i']['size']) + "}"


def lookup_type(val):
    type_str = str(val.type)
    if type_str.startswith('const '): type_str = type_str[6:]
    if type_str.endswith(' *'): type_str = type_str[:-2]
    if type_str == 'iString':
        return StringPrinter(val)
    if type_str.startswith('iRange'):
        return RangePrinter(val)
    if type_str == 'iInt2':
        return Int2Printer(val)
    if type_str == 'iRect':
        return RectPrinter(val)
    return None


gdb.pretty_printers.append(lookup_type)

