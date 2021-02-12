#!/bin/ruby

require 'xz'

Unmap_Chars = -> m do
    m.each_char.map do
        case _1.ord
            when 0..31; _1.ord + 1072
            when    32; 1105
            when    33; 45
            when    34; 10
        end .chr(Encoding::UTF_8)
    end * ''
end

N = Struct.new :letter, :is_last, :first_child, :next_sibling
T = N.new

Insert = -> s do
    t = T
    s.each_char do | c |
        if t.first_child == nil
            t = t.first_child = N.new c
            next
        end
        t = t.first_child
        while t.letter != c && t.next_sibling != nil
            t = t.next_sibling
        end
        (t = t.next_sibling = N.new c) if t.letter != c
    end
    t.is_last = true
end

Unmap_Chars.(XZ.decompress(File.read 'dictionary.txt.mapped_chars.xz')).split.each do Insert.(_1) end

Find_Prefix = -> s do
    t = T
    s.each_char do | c |
        return if (t = t.first_child).nil?
        while t.letter != c && t.next_sibling != nil
            t = t.next_sibling
        end
        return if t.letter != c
    end
    t
end

Traversal = -> t, s do
    return unless t
    puts "\t" + (s + t.letter) if t.is_last
    Traversal.(t.first_child,  s + t.letter)
    Traversal.(t.next_sibling, s)
end

H = <<~HELP
    \tInvalid Input
    \tOnly lowercase russian letters and '-' are allowed
    \tEnter 'q' for exit
HELP

loop do
    print 'Enter Prefix: '
    (puts "\n" + H; next) if (s = gets).nil?
    exit if (s = s.strip) == ?q
    (puts H; next) unless s != '' &&
                          s.each_char.all? { [*(?а..?я), ?ё, ?-].include? _1 }
    (puts "\tPrefix Not Found"; next) unless t = Find_Prefix.(s)
    puts "\t" + s if t.is_last
    Traversal.(t.first_child, s)
end
