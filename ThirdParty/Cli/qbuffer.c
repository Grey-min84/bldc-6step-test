/*
 * qbuffer.c
 *
 *  Created on: 2020. 12. 11.
 *      Author: baram
 */


#include "qbuffer.h"





void qbufferInit(void)
{

}

uint8_t qbufferCreate(qbuffer_t *p_node, uint8_t *p_buf, uint32_t length)
{
  uint8_t ret = 1;

  p_node->in    = 0;
  p_node->out   = 0;
  p_node->len   = length;
  p_node->p_buf = p_buf;

  return ret;
}

uint8_t qbufferWrite(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
{
  uint8_t ret = 1;
  uint32_t next_in;


  for (int i=0; i<length; i++)
  {
    next_in = (p_node->in + 1) % p_node->len;

    if (next_in != p_node->out)
    {
      if (p_node->p_buf != NULL)
      {
        p_node->p_buf[p_node->in] = p_data[i];
      }
      p_node->in = next_in;
    }
    else
    {
      ret = 0;
      break;
    }
  }

  return ret;
}

uint8_t qbufferRead(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
{
  uint8_t ret = 1;


  for (int i=0; i<length; i++)
  {
    if (p_node->p_buf != NULL)
    {
      p_data[i] = p_node->p_buf[p_node->out];
    }

    if (p_node->out != p_node->in)
    {
      p_node->out = (p_node->out + 1) % p_node->len;
    }
    else
    {
      ret = 0;
      break;
    }
  }

  return ret;
}

uint32_t qbufferAvailable(qbuffer_t *p_node)
{
  uint32_t ret;


  ret = (p_node->in - p_node->out) % p_node->len;

  return ret;
}

void qbufferFlush(qbuffer_t *p_node)
{
  p_node->in  = 0;
  p_node->out = 0;
}
