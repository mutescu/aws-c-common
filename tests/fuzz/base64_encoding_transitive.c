/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <aws/common/encoding.h>

#include <assert.h>

/* NOLINTNEXTLINE(readability-identifier-naming) */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {

    struct aws_allocator *allocator = aws_default_allocator();

    size_t output_size = 0;
    int result = aws_base64_compute_encoded_len(size, &output_size);
    assert(result == AWS_OP_SUCCESS);

    struct aws_byte_buf to_encode = aws_byte_buf_from_array(data, size);

    struct aws_byte_buf encode_output;
    result = aws_byte_buf_init(allocator, &encode_output, output_size);
    assert(result == AWS_OP_SUCCESS);

    result = aws_base64_encode(&to_encode, &encode_output);
    assert(result == AWS_OP_SUCCESS);
    --encode_output.len; /* Remove null terminator */

    result = aws_base64_compute_decoded_len((const char *)encode_output.buffer, encode_output.len, &output_size);
    assert(result == AWS_OP_SUCCESS);
    assert(output_size == size);

    struct aws_byte_buf decode_output;
    result = aws_byte_buf_init(allocator, &decode_output, output_size);
    assert(result == AWS_OP_SUCCESS);

    result = aws_base64_decode(&encode_output, &decode_output);
    assert(result == AWS_OP_SUCCESS);
    assert(output_size == decode_output.len);
    assert(memcmp(decode_output.buffer, data, size) == 0);

    aws_byte_buf_clean_up(&encode_output);
    aws_byte_buf_clean_up(&decode_output);

    return 0;
}
