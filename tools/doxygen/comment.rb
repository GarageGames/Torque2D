#! /usr/bin/ruby -w

count = 0
while s = gets
   if s =~ /(ConsoleMethod|ConsoleFunction)/

      count = count + 1

      # ConsoleMethod(class, method, void, 2, 3, "(args) Set whether ...\n"
      #               "@param setting Default is true.\n"
      #               ...

      # break into 3 parts (shown with brackets here)
      # [ConsoleMethod(class, method, void, 2, 3, ]"[(args)] [Set whether ...\n" <all lines after>]
      if s =~ /([^"]*)"([^\)]*\)) ?(.*)/
	 found = true
         start = $1;
         args = $2;
         docs = $3;

      # parse same as above but missing the (args) (including parens)

      elsif s =~ /([^"]*)" ?(.*)/
	 found = true
         start = $1;
	 args = nil;
         docs = $2;
         warn "no params found here: '" + s + "'"
      else
         found = false
      end

      if found
	 # seemed that using 'docs' instead of 'cookedDocs' was overwriting internal ($2 or $3) buffers?
	 cookedDocs = docs;

	 # remove the dash in front of some brief descriptions, like " - Gets whether picking..."
	 cookedDocs.sub!(/^\s*-\s*/, '');

         while s = gets
            if s =~ /^\s*\{/
	       # done finding comments.  ok to eat the "{"
	       break
	    else
	       s.chomp!
               cookedDocs = cookedDocs + "\n" + s
	    end
         end

         # description string lines start with lots of white space.  change to 4 spaces.
	 cookedDocs.gsub!(/[ \t]{4,}/, '    ');
         # remove literal \n from end of comments (in the string)
         cookedDocs.gsub!('\n', '');
         # remove [")] -- quote then paren -- from the last line of the comments (and any whitespace between).
	 cookedDocs.gsub!(/"\s*\)/, '');
         # finally remove end quotes on the lines
	 cookedDocs.gsub!('"', '');

	 cookedStart = start
         # remove white space before the string comment begins
         cookedStart.sub!(/\s*$/, '')

         # sanity check
         # puts "start " + cookedStart;
         # puts "args " + args if args
         # puts "docs " + cookedDocs;

	 puts '/*! ' + cookedDocs + "\n*/"
	 puts cookedStart + " " + (args ? args : "(...)") + ')'
	 puts '{'

      else
         warn "too dumb to parse '" + s + "'"
      end
   else
      puts s
   end
end

warn( "number of lines was " + count.to_s )

