
(load (merge-pathnames #P"build-cando.lisp" *load-pathname*))
(asdf:load-asd (merge-pathnames #P"build-cando-jupyter.asd" *load-pathname*))