;; Auto-generated. Do not edit!


(when (boundp 'encoder::robotencoder)
  (if (not (find-package "ENCODER"))
    (make-package "ENCODER"))
  (shadow 'robotencoder (find-package "ENCODER")))
(unless (find-package "ENCODER::ROBOTENCODER")
  (make-package "ENCODER::ROBOTENCODER"))

(in-package "ROS")
;;//! \htmlinclude robotencoder.msg.html


(defclass encoder::robotencoder
  :super ros::object
  :slots (_leftencoder _rightencoder _seq ))

(defmethod encoder::robotencoder
  (:init
   (&key
    ((:leftencoder __leftencoder) 0)
    ((:rightencoder __rightencoder) 0)
    ((:seq __seq) 0)
    )
   (send-super :init)
   (setq _leftencoder (round __leftencoder))
   (setq _rightencoder (round __rightencoder))
   (setq _seq (round __seq))
   self)
  (:leftencoder
   (&optional __leftencoder)
   (if __leftencoder (setq _leftencoder __leftencoder)) _leftencoder)
  (:rightencoder
   (&optional __rightencoder)
   (if __rightencoder (setq _rightencoder __rightencoder)) _rightencoder)
  (:seq
   (&optional __seq)
   (if __seq (setq _seq __seq)) _seq)
  (:serialization-length
   ()
   (+
    ;; int32 _leftencoder
    4
    ;; int32 _rightencoder
    4
    ;; uint32 _seq
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; int32 _leftencoder
       (write-long _leftencoder s)
     ;; int32 _rightencoder
       (write-long _rightencoder s)
     ;; uint32 _seq
       (write-long _seq s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; int32 _leftencoder
     (setq _leftencoder (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _rightencoder
     (setq _rightencoder (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint32 _seq
     (setq _seq (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get encoder::robotencoder :md5sum-) "2622be2aa7904b3e81edff5d7971b517")
(setf (get encoder::robotencoder :datatype-) "encoder/robotencoder")
(setf (get encoder::robotencoder :definition-)
      "int32 leftencoder
int32 rightencoder
uint32 seq
")



(provide :encoder/robotencoder "2622be2aa7904b3e81edff5d7971b517")


