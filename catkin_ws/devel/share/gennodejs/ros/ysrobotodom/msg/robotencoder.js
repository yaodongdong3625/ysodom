// Auto-generated. Do not edit!

// (in-package ysrobotodom.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class robotencoder {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.leftencoder = null;
      this.rightencoder = null;
      this.seq = null;
    }
    else {
      if (initObj.hasOwnProperty('leftencoder')) {
        this.leftencoder = initObj.leftencoder
      }
      else {
        this.leftencoder = 0;
      }
      if (initObj.hasOwnProperty('rightencoder')) {
        this.rightencoder = initObj.rightencoder
      }
      else {
        this.rightencoder = 0;
      }
      if (initObj.hasOwnProperty('seq')) {
        this.seq = initObj.seq
      }
      else {
        this.seq = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type robotencoder
    // Serialize message field [leftencoder]
    bufferOffset = _serializer.int32(obj.leftencoder, buffer, bufferOffset);
    // Serialize message field [rightencoder]
    bufferOffset = _serializer.int32(obj.rightencoder, buffer, bufferOffset);
    // Serialize message field [seq]
    bufferOffset = _serializer.uint32(obj.seq, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type robotencoder
    let len;
    let data = new robotencoder(null);
    // Deserialize message field [leftencoder]
    data.leftencoder = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [rightencoder]
    data.rightencoder = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [seq]
    data.seq = _deserializer.uint32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'ysrobotodom/robotencoder';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '2622be2aa7904b3e81edff5d7971b517';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    int32 leftencoder
    int32 rightencoder
    uint32 seq
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new robotencoder(null);
    if (msg.leftencoder !== undefined) {
      resolved.leftencoder = msg.leftencoder;
    }
    else {
      resolved.leftencoder = 0
    }

    if (msg.rightencoder !== undefined) {
      resolved.rightencoder = msg.rightencoder;
    }
    else {
      resolved.rightencoder = 0
    }

    if (msg.seq !== undefined) {
      resolved.seq = msg.seq;
    }
    else {
      resolved.seq = 0
    }

    return resolved;
    }
};

module.exports = robotencoder;
