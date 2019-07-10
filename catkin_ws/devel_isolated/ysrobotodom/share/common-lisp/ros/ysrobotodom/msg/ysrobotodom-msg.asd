
(cl:in-package :asdf)

(defsystem "ysrobotodom-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "robotencoder" :depends-on ("_package_robotencoder"))
    (:file "_package_robotencoder" :depends-on ("_package"))
  ))