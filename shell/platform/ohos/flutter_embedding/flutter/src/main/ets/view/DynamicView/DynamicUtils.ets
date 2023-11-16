import { DVModelParameters } from './dynamicView';

export class DynamicUtils {
  static getParams(params: DVModelParameters, element: string): string | ESObject {
    let params2 = params as Record<string, ESObject>;
    return params2[element];
  }

  static setParams(params: DVModelParameters, key: string, element: ESObject): void {
    let params2 = params as Record<string, ESObject>;
    params2[key] = element;
  }
}
