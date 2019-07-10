; Auto-generated. Do not edit!


(cl:in-package encoder-msg)


;//! \htmlinclude robotencoder.msg.html

(cl:defclass <robotencoder> (roslisp-msg-protocol:ros-message)
  ((leftencoder
    :reader leftencoder
    :initarg :leftencoder
    :type cl:integer
    :initform 0)
   (rightencoder
    :reader rightencoder
    :initarg :rightencoder
    :type cl:integer
    :initform 0)
   (seq
    :reader seq
    :initarg :seq
    :type cl:integer
    :initform 0))
)

(cl:defclass robotencoder (<robotencoder>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <robotencoder>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'robotencoder)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name encoder-msg:<robotencoder> is deprecated: use encoder-msg:robotencoder instead.")))

(cl:ensure-generic-function 'leftencoder-val :lambda-list '(m))
(cl:defmethod leftencoder-val ((m <robotencoder>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader encoder-msg:leftencoder-val is deprecated.  Use encoder-msg:leftencoder instead.")
  (leftencoder m))

(cl:ensure-generic-function 'rightencoder-val :lambda-list '(m))
(cl:defmethod rightencoder-val ((m <robotencoder>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader encoder-msg:rightencoder-val is deprecated.  Use encoder-msg:rightencoder instead.")
  (rightencoder m))

(cl:ensure-generic-function 'seq-val :lambda-list '(m))
(cl:defmethod seq-val ((m <robotencoder>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader encoder-msg:seq-val is deprecated.  Use encoder-msg:seq instead.")
  (seq m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <robotencoder>) ostream)
  "Serializes a message object of type '<robotencoder>"
  (cl:let* ((signed (cl:slot-value msg 'leftencoder)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'rightencoder)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'seq)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'seq)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'seq)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'seq)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <robotencoder>) istream)
  "Deserializes a message object of type '<robotencoder>"
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'leftencoder) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'rightencoder) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'seq)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'seq)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'seq)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'seq)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<robotencoder>)))
  "Returns string type for a message object of type '<robotencoder>"
  "encoder/robotencoder")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'robotencoder)))
  "Returns string type for a message object of type 'robotencoder"
  "encoder/robotencoder")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<robotencoder>)))
  "Returns md5sum for a message object of type '<robotencoder>"
  "2622be2aa7904b3e81edff5d7971b517")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'robotencoder)))
  "Returns md5sum for a message object of type 'robotencoder"
  "2622be2aa7904b3e81edff5d7971b517")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<robotencoder>)))
  "Returns full string definition for message of type '<robotencoder>"
  (cl:format cl:nil "int32 leftencoder~%int32 rightencoder~%uint32 seq~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'robotencoder)))
  "Returns full string definition for message of type 'robotencoder"
  (cl:format cl:nil "int32 leftencoder~%int32 rightencoder~%uint32 seq~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <robotencoder>))
  (cl:+ 0
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <robotencoder>))
  "Converts a ROS message object to a list"
  (cl:list 'robotencoder
    (cl:cons ':leftencoder (leftencoder msg))
    (cl:cons ':rightencoder (rightencoder msg))
    (cl:cons ':seq (seq msg))
))
