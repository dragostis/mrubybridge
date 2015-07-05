left = Identity.complex 2, 2.5, [1, 2, 3], "foo"
right = [1, 2, 3]

raise "#{left} != #{right}" unless left == right